from pathlib import Path

path = Path("src/slic3r/GUI/Tab.cpp")
text = path.read_text(encoding="utf-8")

def replace_once(old: str, new: str) -> None:
    global text
    count = text.count(old)
    if count != 1:
        raise RuntimeError(f"Expected one match, found {count}: {old[:80]!r}")
    text = text.replace(old, new, 1)

replace_once(
    '        auto select_spoolman_filament = [this, spoolman_group](wxWindow* parent) {',
    '''        spoolman_group->append_single_option_line("spoolman_filament_id");

        ConfigOptionsGroupWkp spoolman_group_wk(spoolman_group);
        Line select_spoolman_line { "", "" };
        select_spoolman_line.full_width = 1;
        select_spoolman_line.widget = [this, spoolman_group_wk](wxWindow* parent) {'''
)
replace_once(
    '''            auto btn = new ScalableButton(parent, wxID_ANY, "search", _L("Select from Spoolman") + " " + dots,
                                          wxDefaultSize, wxDefaultPosition, wxBU_LEFT | wxBU_EXACTFIT, true);
            btn->SetFont(wxGetApp().normal_font());
            sizer->Add(btn);

            btn->Bind(wxEVT_BUTTON, [this, parent, spoolman_group](wxCommandEvent&) {''',
    '''            auto btn = new wxButton(parent, wxID_ANY, _L("Select from Spoolman") + " " + dots);
            btn->SetFont(wxGetApp().normal_font());
            sizer->Add(btn, 0, wxALIGN_LEFT);

            btn->Bind(wxEVT_BUTTON, [this, parent, spoolman_group_wk](wxCommandEvent&) {'''
)
replace_once(
    '''                    spoolman_group->set_value("spoolman_filament_id", entries[choice].second, true);
                    if (Field* field = spoolman_group->get_field("spoolman_filament_id"))
                        field->field_changed();''',
    '''                    if (auto group = spoolman_group_wk.lock(); group) {
                        group->set_value("spoolman_filament_id", entries[choice].second, true);
                        if (Field* field = group->get_field("spoolman_filament_id"))
                            field->field_changed();
                    }'''
)
replace_once(
    '''        Option spoolman_filament_option = spoolman_group->get_option("spoolman_filament_id");
        Line spoolman_filament_line = spoolman_group->create_single_option_line(spoolman_filament_option);
        spoolman_filament_line.append_widget(select_spoolman_filament);
        spoolman_group->append_line(spoolman_filament_line);''',
    '        spoolman_group->append_line(select_spoolman_line);'
)

path.write_text(text, encoding="utf-8", newline="\n")
