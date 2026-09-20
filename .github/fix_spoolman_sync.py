from pathlib import Path


def replace_once(text: str, old: str, new: str, label: str) -> str:
    if text.count(old) != 1:
        raise RuntimeError(f"Expected one {label}, found {text.count(old)}")
    return text.replace(old, new, 1)


tab_path = Path("src/slic3r/GUI/Tab.cpp")
tab = tab_path.read_text(encoding="utf-8")
picker_start = tab.index("        ConfigOptionsGroupWkp spoolman_group_wk(spoolman_group);")
picker_end = tab.index("        spoolman_group->m_on_change", picker_start)
tab = tab[:picker_start] + tab[picker_end:]
tab_path.write_text(tab, encoding="utf-8")


plater_path = Path("src/slic3r/GUI/Plater.cpp")
plater = plater_path.read_text(encoding="utf-8")
plater = replace_once(
    plater,
    "Preset* resolve_filament_preset(PresetBundle* preset_bundle, \n"
    "    const std::string& filament_name, const std::string& filament_type, int spool_id)",
    "Preset* resolve_filament_preset(PresetBundle* preset_bundle,\n"
    "    const std::string& filament_name, const std::string& filament_type,\n"
    "    const std::string& filament_color, int spool_id)",
    "resolver signature",
)

fallback_marker = "\n    auto to_lower = [](std::string s) {"
if plater.count(fallback_marker) != 1:
    raise RuntimeError("Could not locate the name-matching fallback")

spoolman_mapping = r'''

    // The U1 does not always expose a Spoolman spool ID. Resolve the
    // filament by material and colour and use the permanent filament ID
    // stored in the local Orca profile instead.
    std::string spoolman_url;
    for (const Preset& preset : preset_bundle->filaments) {
        if (!preset.config.has("spoolman_url"))
            continue;
        spoolman_url = preset.config.opt_string("spoolman_url", 0u);
        if (!spoolman_url.empty())
            break;
    }
    while (!spoolman_url.empty() && spoolman_url.back() == '/')
        spoolman_url.pop_back();

    if (!spoolman_url.empty() && !filament_type.empty() && !filament_color.empty()) {
        std::string response;
        std::string request_error;
        unsigned status = 0;
        auto http = Http::get(spoolman_url + "/api/v1/filament");
        http.timeout_connect(3)
            .timeout_max(8)
            .on_complete([&](std::string body, unsigned http_status) {
                response = std::move(body);
                status = http_status;
            })
            .on_error([&](std::string body, std::string error, unsigned http_status) {
                response = std::move(body);
                request_error = std::move(error);
                status = http_status;
            })
            .perform_sync();

        if (status == 200) {
            try {
                const nlohmann::json filaments = nlohmann::json::parse(response);
                if (filaments.is_array()) {
                    auto normalized = [](std::string value) {
                        std::transform(value.begin(), value.end(), value.begin(),
                                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                        if (!value.empty() && value.front() == '#')
                            value.erase(value.begin());
                        if (value.size() == 8)
                            value.resize(6);
                        return value;
                    };
                    const std::string wanted_type  = normalized(filament_type);
                    const std::string wanted_color = normalized(filament_color);
                    const std::string machine_name = normalized(filament_name);

                    for (Preset& preset : preset_bundle->filaments) {
                        if (!preset.is_compatible || !preset.config.has("spoolman_filament_id"))
                            continue;
                        const std::string configured_id = preset.config.opt_string("spoolman_filament_id", 0u);
                        if (configured_id.empty())
                            continue;

                        for (const nlohmann::json& filament : filaments) {
                            if (!filament.contains("id") || !filament["id"].is_number_integer() ||
                                std::to_string(filament["id"].get<int>()) != configured_id)
                                continue;

                            const auto text = [&filament](const char* key) {
                                return filament.contains(key) && filament[key].is_string()
                                           ? filament[key].get<std::string>() : std::string();
                            };
                            const std::string material = normalized(text("material"));
                            const std::string color    = normalized(text("color_hex"));
                            if (material != wanted_type || color != wanted_color)
                                continue;

                            std::string vendor;
                            if (filament.contains("vendor") && filament["vendor"].is_object() &&
                                filament["vendor"].contains("name") && filament["vendor"]["name"].is_string())
                                vendor = normalized(filament["vendor"]["name"].get<std::string>());
                            if (!vendor.empty() && machine_name.find(vendor) == std::string::npos)
                                continue;

                            BOOST_LOG_TRIVIAL(info) << "Spoolman profile resolved by material/color: filament="
                                                    << configured_id << " profile=" << preset.name;
                            return &preset;
                        }
                    }
                }
            } catch (const std::exception& e) {
                BOOST_LOG_TRIVIAL(warning) << "Invalid Spoolman filament list: " << e.what();
            }
        } else {
            BOOST_LOG_TRIVIAL(warning) << "Could not load Spoolman filaments (HTTP " << status
                                       << "): " << request_error;
        }
    }
'''
plater = plater.replace(fallback_marker, spoolman_mapping + fallback_marker, 1)

old_call = (
    "            Preset* matched = resolve_filament_preset(preset_bundle, syncedData[i].m_name, syncedData[i].m_type,\n"
    "                                                      syncedData[i].m_spool_id);"
)
new_call = (
    "            const wxColour synced_color = getMainColor(syncedData[i].m_color);\n"
    "            Preset* matched = resolve_filament_preset(\n"
    "                preset_bundle, syncedData[i].m_name, syncedData[i].m_type,\n"
    "                into_u8(synced_color.GetAsString(wxC2S_HTML_SYNTAX)), syncedData[i].m_spool_id);"
)
plater = replace_once(plater, old_call, new_call, "sync resolver call")
plater_path.write_text(plater, encoding="utf-8")

