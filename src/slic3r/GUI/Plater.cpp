Y��x-���jם��i��+��j[h��ܢ��߯w��6��o+^����ם#include "Plater.hpp"
#include "MixedFilamentDialog.hpp"
#include "MixedFilamentBatchDialog.hpp"
#include "MixedGradientSelector.hpp"
#include "MixedColorMatchPanel.hpp"
#include "MixedFilamentBadge.hpp"
#include "MixedFilamentColorMapPanel.hpp"
#include "MixedColorMatchHelpers.hpp"
#include "libslic3r/Config.hpp"
#include "libslic3r/MixedFilament.hpp"
#include "libslic3r/filament_mixer.h"
#include "common_func/common_func.hpp"

#include <atomic>
#include <cstddef>
#include <array>
#include <cctype>
#include <cstdlib>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <numeric>
#include <memory>
#include <limits>
#include <thread>
#include <vector>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <future>
#include <functional>
#include <sstream>
#include <utility>
#include <boost/algorithm/string.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/log/trivial.hpp>
#include <boost/nowide/convert.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/bmpcbox.h>
#include <wx/statbox.h>
#include <wx/statbmp.h>
#include <wx/filedlg.h>
#include <wx/dnd.h>
#include <wx/progdlg.h>
#include <wx/string.h>
#include <wx/wupdlock.h>
#include <wx/numdlg.h>
#include <wx/debug.h>
#include <wx/busyinfo.h>
#include <wx/dcbuffer.h>
#include <wx/scrolwin.h>
#include <wx/event.h>
#include <wx/wrapsizer.h>
#include <wx/choice.h>
#include <wx/gauge.h>
#include <wx/slider.h>
#include <wx/textctrl.h>
#include <wx/weakref.h>
#ifdef _WIN32
#include <wx/richtooltip.h>
#include <wx/custombgwin.h>
#include <wx/popupwin.h>
#endif
#include <wx/clrpicker.h>
#include <wx/spinctrl.h>
#include <wx/timer.h>
#include <wx/tokenzr.h>
#include <wx/aui/aui.h>

#include "libslic3r/libslic3r.h"
#include "libslic3r/Format/STL.hpp"
#include "libslic3r/Format/STEP.hpp"
#include "libslic3r/Format/AMF.hpp"
//#include "libslic3r/Format/3mf.hpp"
#include "libslic3r/Format/bbs_3mf.hpp"
#include "libslic3r/GCode/ThumbnailData.hpp"
#include "libslic3r/Model.hpp"
#include "libslic3r/SLA/Hollowing.hpp"
#include "libslic3r/SLA/SupportPoint.hpp"
#include "slic3r/Utils/Http.hpp"
#include <nlohmann/json.hpp>
#include "libslic3r/SLA/ReprojectPointsOnMesh.hpp"
#include "libslic3r/Polygon.hpp"
#include "libslic3r/Print.hpp"
#include "libslic3r/PrintConfig.hpp"
#include "libslic3r/SLAPrint.hpp"
#include "libslic3r/Utils.hpp"
#include "libslic3r/PresetBundle.hpp"
#include "libslic3r/ClipperUtils.hpp"
#include "libslic3r/FilamentHotBedNozzleRules.hpp"

// For stl export
#include "libslic3r/CSGMesh/ModelToCSGMesh.hpp"
#include "libslic3r/CSGMesh/PerformCSGMeshBooleans.hpp"

#include "GUI.hpp"
#include "GUI_App.hpp"
#include "GUI_ObjectList.hpp"
#include "GUI_Utils.hpp"
#include "GUI_Factories.hpp"
#include "wxExtensions.hpp"
#include "MainFrame.hpp"
#include "format.hpp"
#include "3DScene.hpp"
#include "GLCanvas3D.hpp"
#include "Selection.hpp"
#include "GLToolbar.hpp"
#include "GUI_Preview.hpp"
#include "3DBed.hpp"
#include "PartPlate.hpp"
#include "Camera.hpp"
#include "Mouse3DController.hpp"
#include "Tab.hpp"
#include "Jobs/OrientJob.hpp"
#include "Jobs/ArrangeJob.hpp"
#include "Jobs/FillBedJob.hpp"
#include "Jobs/RotoptimizeJob.hpp"
#include "Jobs/SLAImportJob.hpp"
#include "Jobs/SLAImportDialog.hpp"
#include "Jobs/PrintJob.hpp"
#include "Jobs/NotificationProgressIndicator.hpp"
#include "Jobs/PlaterWorker.hpp"
#include "Jobs/BoostThreadWorker.hpp"
#include "BackgroundSlicingProcess.hpp"
#include "SelectMachine.hpp"
#include "SendMultiMachinePage.hpp"
#include "SendToPrinter.hpp"
#include "PublishDialog.hpp"
#include "ModelMall.hpp"
#include "ConfigWizard.hpp"
#include "../Utils/ASCIIFolding.hpp"
#include "../Utils/ColorSpaceConvert.hpp"
#include "../Utils/FixModelByWin10.hpp"
#include "../Utils/UndoRedo.hpp"
#include "../Utils/PresetUpdater.hpp"
#include "../Utils/Process.hpp"
#include "RemovableDriveManager.hpp"
#include "InstanceCheck.hpp"
#include "NotificationManager.hpp"
#include "PresetComboBoxes.hpp"
#include "MsgDialog.hpp"
#include "ProjectDirtyStateManager.hpp"
#include "Gizmos/GLGizmoSimplify.hpp" // create suggestion notification
#include "Gizmos/GLGizmoSVG.hpp" // Drop SVG file
#include "Gizmos/GizmoObjectManipulation.hpp"

// BBS
#include "Widgets/ProgressDialog.hpp"
#include "BBLStatusBar.hpp"
#include "BitmapCache.hpp"
#include "ParamsDialog.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/RoundedRectangle.hpp"
#include "Widgets/RadioGroup.hpp"
#include "Widgets/DialogButtons.hpp"
#include "Widgets/CheckBox.hpp"
#include "Widgets/Button.hpp"

#include "GUI_ObjectTable.hpp"
#include "libslic3r/Thread.hpp"

#ifdef __APPLE__
#include "Gizmos/GLGizmosManager.hpp"
#endif // __APPLE__

#include <libslic3r/CutUtils.hpp>
#include <wx/glcanvas.h>    // Needs to be last because reasons :-/
#include <libslic3r/miniz_extension.hpp>
#include "WipeTowerDialog.hpp"
#include "ObjColorDialog.hpp"

#include "libslic3r/CustomGCode.hpp"
#include "libslic3r/Platform.hpp"
#include "nlohmann/json.hpp"

#include "PhysicalPrinterDialog.hpp"
#include "PrintHostDialogs.hpp"
#include "PlateSettingsDialog.hpp"
#include "DailyTips.hpp"
#include "CreatePresetsDialog.hpp"
#include "FileArchiveDialog.hpp"
#include "StepMeshDialog.hpp"
#include "CloneDialog.hpp"
#include "WebPreprintDialog.hpp"

#include "filamentsync/SyncConfirmDialog.hpp"
#include "filamentsync/SyncFilamentColorDialog.hpp"

#include "sentry_wrapper/SentryWrapper.hpp"
#include <chrono>

using boost::optional;
namespace fs = boost::filesystem;
using Slic3r::_3DScene;
using Slic3r::Preset;
using Slic3r::GUI::format_wxstr;
using namespace nlohmann;

static const std::pair<unsigned int, unsigned int> THUMBNAIL_SIZE_3MF = { 512, 512 };

namespace Slic3r {
namespace GUI {

// Defensive UTF-8 translation helper.
//
// I18N::translate_utf8() (and the _u8L macro) returns std::string constructed
// from a chain of temporaries: wxGetTranslation(...).ToUTF8().data(). The
// C++ standard says this is safe — the temporaries live until the end of the
// full expression, which includes the std::string copy construction. In
// practice, however, MSVC has historically mis-optimized such chains under
// /O2 (the buffer sometimes gets destroyed before the std::string reads it),
// producing reads from freed memory. The classic #648 NULL-FILE* crash was a
// similar temporary-chain pattern backfiring. Naming each step as a local
// variable makes the lifetime unambiguous to the optimizer and avoids the
// whole class of bugs across compilers and build flags.
//
// Null defenses: wxGetTranslation / wxString(nullptr, wxConvUTF8) are UB on
// null input, and std::string(nullptr, 0) is UB even with zero length, so we
// short-circuit both. Returns "" on null input.
static std::string tr_u8(const char* s)
{
    if (s == nullptr)
        return std::string();
    const wxString        ws  = _L(s);
    const wxScopedCharBuffer buf = ws.utf8_str();
    if (buf.data() == nullptr)
        return std::string();
    return std::string(buf.data(), buf.length());
}

// Build a user-facing label for a single filament slot, in the form
// "[n] PresetName". Falls back to "[n] (unknown)" if the preset can't be
// resolved — same defensive style as the null checks in check_filament_temp_mixing.
static std::string filament_display_label(int slot_1based)
{
    const int      slot_0_based = slot_1based - 1;
    PresetBundle*  bundle       = wxGetApp().preset_bundle;
    std::string    name         = tr_u8("unknown");
    if (bundle != nullptr
        && slot_0_based >= 0
        && slot_0_based < static_cast<int>(bundle->filament_presets.size()))
    {
        const Preset* preset = bundle->filaments.find_preset(bundle->filament_presets[slot_0_based], true);
        if (preset != nullptr)
            name = preset->name;
    }
    return std::string("[") + std::to_string(slot_1based) + "] " + name;
}

// Compose a single comma-separated line of "[n] Preset" labels from a list
// of 1-based slot numbers. Used inside High/Low temperature group lines.
static std::string format_filament_slot_list(const std::vector<int>& slots_1based)
{
    std::string out;
    for (size_t i = 0; i < slots_1based.size(); ++i)
    {
        if (i != 0)
            out += ", ";
        out += filament_display_label(slots_1based[i]);
    }
    return out;
}

// Append the High/Low temperature grouping block (two lines, only groups that
// are non-empty) to `out`. Called by both the single-plate and slice-all
// text builders so the layout stays identical.
static void append_filament_temp_mixing_groups(std::string& out, const Plater::FilamentTempMixingDetail& detail)
{
    if (!detail.high_temp_slots_1based.empty())
    {
        out += tr_u8("High temperature:");
        out += " ";
        out += format_filament_slot_list(detail.high_temp_slots_1based);
        out += "\n";
    }
    if (!detail.low_temp_slots_1based.empty())
    {
        out += tr_u8("Low temperature:");
        out += " ";
        out += format_filament_slot_list(detail.low_temp_slots_1based);
        out += "\n";
    }
}

static std::string filament_temp_mixing_warning_text(const Plater::FilamentTempMixingDetail& detail)
{
    std::string out = tr_u8("Detected both high and low temperature materials. "
                            "Mixed printing may result in extruder clogging, "
                            "nozzle damage, or layer adhesion issues.");
    out += "\n\n";
    append_filament_temp_mixing_groups(out, detail);
    return out;
}

static std::string filament_temp_mixing_error_text(const Plater::FilamentTempMixingDetail& detail)
{
    std::string out = tr_u8("Detected both high and low temperature materials. "
                            "Mixed printing may result in extruder clogging, "
                            "nozzle damage, or layer adhesion issues.");
    out += "\n\n";
    append_filament_temp_mixing_groups(out, detail);
    out += "\n";
    out += tr_u8("To continue printing, enable \"Allow high/low temperature filament mixing\" in Preferences.");
    return out;
}

// Build the single-line banner for the flow-ratio-zero pre-slice blocker.
// Layout (single line, semicolon-separated):
//   "Flow ratio is 0%, ... no valid toolpath. Filament(s): [1] PLA White; [3] PETG Black. Please set ..."
// Uses std::string += concatenation (no boost::format) to avoid format_error exceptions,
// matching the filament_temp_mixing_*_text blueprints above.
static std::string flow_ratio_zero_error_text(const Plater::FlowRatioZeroDetail& detail)
{
    std::string out = tr_u8("Flow ratio is 0%, resulting in zero extrusion "
                            "and no valid toolpath. ");
    out += tr_u8("Filament(s):");
    out += " ";
    for (size_t i = 0; i < detail.offender_slots_1based.size(); ++i)
    {
        if (i != 0)
            out += "; ";
        out += filament_display_label(detail.offender_slots_1based[i]);
    }
    out += ". ";
    out += tr_u8("Please set the flow ratio to a value greater than 0.");
    return out;
}

// Slice-all variants: list every plate that has a mixing conflict, each with
// its own High/Low grouping. Plates without conflicts are not shown.
static std::string filament_temp_mixing_warning_text_slice_all(const std::vector<Plater::PlateMixingInfo>& plates)
{
    std::string out = tr_u8("The following plates contain mixed high and low temperature materials:");
    out += "\n\n";
    for (const Plater::PlateMixingInfo& info : plates)
    {
        out += tr_u8("Plate");
        out += " " + std::to_string(info.plate_index_1based) + "\n";
        append_filament_temp_mixing_groups(out, info.detail);
        out += "\n";
    }
    return out;
}

static std::string filament_temp_mixing_error_text_slice_all(const std::vector<Plater::PlateMixingInfo>& plates)
{
    std::string out = tr_u8("The following plates contain mixed high and low temperature materials:");
    out += "\n\n";
    for (const Plater::PlateMixingInfo& info : plates)
    {
        out += tr_u8("Plate");
        out += " " + std::to_string(info.plate_index_1based) + "\n";
        append_filament_temp_mixing_groups(out, info.detail);
        out += "\n";
    }
    out += tr_u8("To continue printing, enable \"Allow high/low temperature filament mixing\" in Preferences.");
    return out;
}

/// \brief Compose error text for unsupported filaments on the Cool Steel Plate.
/// \param[in] unsupported_slots_1_based  Offending filament slots (1-based).
/// \return Single-line string in the form
///         "Cool Steel Plate is not recommended for printing [filaments]. ..."
static std::string cold_plate_error_text(
    const std::vector<int>&  unsupported_slots_1_based)
{
    return Slic3r::GUI::format(
        _u8L("The Cool Steel Plate is not recommended for %1%. To continue printing, set the bed temperature above 0°C for this filament."),
        format_filament_slot_list(unsupported_slots_1_based));
}

/// \brief Compose TPU serious-warning text for the Cool Steel Plate.
/// \param[in] tpu_slots_1_based   TPU filament slots (1-based).
static std::string cold_plate_serious_warning_text(
    const std::vector<int>&  tpu_slots_1_based)
{
    return Slic3r::GUI::format(
        _u8L("The Cool Steel Plate is not recommended for %1%. It may be hard to remove. Use a textured PEI plate or heat the bed."),
        format_filament_slot_list(tpu_slots_1_based));
}

static bool model_object_is_on_plate(PartPlate* plate, size_t obj_idx, const ModelObject* model_object)
{
    if (model_object == nullptr)
        return false;

    if (plate == nullptr)
        return false;

    const int object_index = static_cast<int>(obj_idx);
    const int instance_count = static_cast<int>(model_object->instances.size());
    for (int instance_index = 0; instance_index < instance_count; ++instance_index)
    {
        if (plate->contain_instance(object_index, instance_index))
            return true;
    }

    return false;
}

static void collect_filament_slots_from_config(
    const DynamicPrintConfig& config,
    int num_filaments, std::set<int>& used_slots_0_based)
{
    // Support/feature filaments
    static const std::vector<const char*> feature_keys = {
        "support_filament",
        "support_interface_filament",
        "wall_filament",
        "sparse_infill_filament",
        "solid_infill_filament",
        "wipe_tower_filament"
    };
    for (const char* key : feature_keys)
    {
        const ConfigOptionInt* option = config.option<ConfigOptionInt>(key);
        if (option != nullptr && option->value >= 1 && option->value <= num_filaments)
            used_slots_0_based.insert(option->value - 1);
    }

    // Primary filament (extruder)
    const ConfigOptionInt* extruder_option = config.option<ConfigOptionInt>("extruder");
    if (extruder_option != nullptr && extruder_option->value >= 1 && extruder_option->value <= num_filaments)
        used_slots_0_based.insert(extruder_option->value - 1);
}

static void collect_filament_slots_from_model_config(
    const ModelConfigObject& config,
    int num_filaments, std::set<int>& used_slots_0_based)
{
    // Primary filament (extruder)
    if (config.has("extruder"))
    {
        const int extruder_id = config.extruder();
        if (extruder_id >= 1 && extruder_id <= num_filaments)
            used_slots_0_based.insert(extruder_id - 1);
    }

    // Support/feature filaments
    static const std::vector<const char*> feature_keys = {
        "support_filament",
        "support_interface_filament",
        "wall_filament",
        "sparse_infill_filament",
        "solid_infill_filament",
        "wipe_tower_filament"
    };
    for (const char* key : feature_keys)
    {
        if (config.has(key))
        {
            const int val = config.opt_int(key);
            if (val >= 1 && val <= num_filaments)
                used_slots_0_based.insert(val - 1);
        }
    }
}

/// \brief Collect the 0-based filament slots actually used by objects on the given plate.
/// \details Mirrors the slot-collection block of Plater::check_filament_temp_mixing so that
///          cold-plate incompatibility checking uses the same definition of "used filament".
///          Includes: plate config, per-object/volume configs, plus Plater working config
///          (wipe_tower / support / wall / infill defaults when any object uses extruder=0).
/// \param[in]  plate                 Non-null target plate.
/// \param[in]  num_filaments         Total number of filaments in the current configuration.
/// \param[in]  plater_working_config The Plater's current working config (this->config()).
/// \param[in]  full_cfg              Merged full config (for resolving the global default extruder).
/// \param[out] used_slots_0_based    Populated with every 0-based filament slot referenced on the plate.
static void collect_used_filament_slots_on_plate(
    PartPlate* plate,
    int num_filaments,
    const DynamicPrintConfig* plater_working_config,
    const DynamicPrintConfig& full_cfg,
    std::set<int>& used_slots_0_based)
{
    if (plate == nullptr || num_filaments <= 0)
        return;

    // Plate-local config
    collect_filament_slots_from_config(*plate->config(), num_filaments, used_slots_0_based);

    // Per-object + per-volume config
    bool uses_default_extruder = false;
    for (size_t obj_idx = 0; obj_idx < wxGetApp().model().objects.size(); ++obj_idx) {
        const ModelObject* model_object = wxGetApp().model().objects[obj_idx];
        if (!model_object_is_on_plate(plate, obj_idx, model_object))
            continue;
        collect_filament_slots_from_model_config(model_object->config, num_filaments, used_slots_0_based);

        if (!model_object->config.has("extruder") || model_object->config.extruder() == 0)
            uses_default_extruder = true;

        for (const ModelVolume* model_volume : model_object->volumes) {
            collect_filament_slots_from_model_config(model_volume->config, num_filaments, used_slots_0_based);
            for (int extruder_id : model_volume->get_extruders()) {
                if (extruder_id >= 1 && extruder_id <= num_filaments)
                    used_slots_0_based.insert(extruder_id - 1);
            }
        }
    }

    // Plater working config — global features (always apply) + feature-specific
    // keys (only when at least one object uses the default extruder).
    if (plater_working_config != nullptr) {
        static const std::vector<const char*> always_collect = {"wipe_tower_filament", "support_filament", "support_interface_filament"};
        for (const char* key : always_collect) {
            const ConfigOptionInt* option = plater_working_config->option<ConfigOptionInt>(key);
            if (option != nullptr && option->value >= 1 && option->value <= num_filaments)
                used_slots_0_based.insert(option->value - 1);
        }

        if (uses_default_extruder) {
            static const std::vector<const char*> default_keys = {"wall_filament", "sparse_infill_filament", "solid_infill_filament"};
            for (const char* key : default_keys) {
                const ConfigOptionInt* option = plater_working_config->option<ConfigOptionInt>(key);
                if (option != nullptr && option->value >= 1 && option->value <= num_filaments)
                    used_slots_0_based.insert(option->value - 1);
            }
        }
    }

    // Resolve the global default extruder if any object on this plate uses extruder=0.
    // plater_working_config does not include "extruder"; read from full_cfg instead.
    if (uses_default_extruder) {
        const ConfigOptionInt* extruder_opt = full_cfg.option<ConfigOptionInt>("extruder");
        if (extruder_opt != nullptr && extruder_opt->value >= 1 && extruder_opt->value <= num_filaments)
            used_slots_0_based.insert(extruder_opt->value - 1);
    }
}

wxDEFINE_EVENT(EVT_SCHEDULE_BACKGROUND_PROCESS,     SimpleEvent);
wxDEFINE_EVENT(EVT_SLICING_UPDATE,                  SlicingStatusEvent);
wxDEFINE_EVENT(EVT_SLICING_COMPLETED,               wxCommandEvent);
wxDEFINE_EVENT(EVT_PROCESS_COMPLETED,               SlicingProcessCompletedEvent);
wxDEFINE_EVENT(EVT_EXPORT_BEGAN,                    wxCommandEvent);
wxDEFINE_EVENT(EVT_EXPORT_FINISHED,                 wxCommandEvent);
wxDEFINE_EVENT(EVT_IMPORT_MODEL_ID,                 wxCommandEvent);
wxDEFINE_EVENT(EVT_DOWNLOAD_PROJECT,                wxCommandEvent);
wxDEFINE_EVENT(EVT_PUBLISH,                         wxCommandEvent);
wxDEFINE_EVENT(EVT_OPEN_PLATESETTINGSDIALOG,        wxCommandEvent);
// BBS: backup & restore
wxDEFINE_EVENT(EVT_RESTORE_PROJECT,                 wxCommandEvent);
wxDEFINE_EVENT(EVT_PRINT_FINISHED,                  wxCommandEvent);
wxDEFINE_EVENT(EVT_SEND_CALIBRATION_FINISHED,       wxCommandEvent);
wxDEFINE_EVENT(EVT_SEND_FINISHED,                   wxCommandEvent);
wxDEFINE_EVENT(EVT_PUBLISH_FINISHED,                wxCommandEvent);
//BBS: repair model
wxDEFINE_EVENT(EVT_REPAIR_MODEL,                    wxCommandEvent);
wxDEFINE_EVENT(EVT_FILAMENT_COLOR_CHANGED,          wxCommandEvent);
wxDEFINE_EVENT(EVT_INSTALL_PLUGIN_NETWORKING,       wxCommandEvent);
wxDEFINE_EVENT(EVT_UPDATE_PLUGINS_WHEN_LAUNCH,       wxCommandEvent);
wxDEFINE_EVENT(EVT_INSTALL_PLUGIN_HINT,             wxCommandEvent);
wxDEFINE_EVENT(EVT_PREVIEW_ONLY_MODE_HINT,          wxCommandEvent);
//BBS: change light/dark mode
wxDEFINE_EVENT(EVT_GLCANVAS_COLOR_MODE_CHANGED,     SimpleEvent);
//BBS: print
wxDEFINE_EVENT(EVT_PRINT_FROM_SDCARD_VIEW,          SimpleEvent);

wxDEFINE_EVENT(EVT_CREATE_FILAMENT, SimpleEvent);
wxDEFINE_EVENT(EVT_MODIFY_FILAMENT, SimpleEvent);
wxDEFINE_EVENT(EVT_FILAMENT_USAGE_CHANGED, SimpleEvent);
wxDEFINE_EVENT(EVT_ADD_FILAMENT, SimpleEvent);
wxDEFINE_EVENT(EVT_DEL_FILAMENT, SimpleEvent);
wxDEFINE_EVENT(EVT_ADD_CUSTOM_FILAMENT, ColorEvent);


#define PRINTER_THUMBNAIL_SIZE (wxSize(FromDIP(48), FromDIP(48)))
#define PRINTER_THUMBNAIL_SIZE_SMALL (wxSize(FromDIP(32), FromDIP(32)))
#define PRINTER_PANEL_SIZE_SMALL (wxSize(FromDIP(98), FromDIP(68)))
#define PRINTER_PANEL_SIZE_WIDEN (wxSize(FromDIP(136), FromDIP(68)))
#define PRINTER_PANEL_SIZE (wxSize(FromDIP(98), FromDIP(98)))

// Nozzle diameter selection when multiple diameters are reported (e.g. U1 sync).
// diameters_raw: list from device (may have duplicates or fewer than 4). Dedup and full-list logic inside.
namespace {
class NozzleDiameterSelectDialog : public DPIDialog
{
    RadioGroup* m_radio = nullptr;
    std::vector<std::string> m_diameters;

public:
    NozzleDiameterSelectDialog(wxWindow* parent, const wxString& message, const wxString& caption,
                               const std::vector<std::string>& diameters_raw)
        : DPIDialog(parent, wxID_ANY, caption, wxDefaultPosition, wxDefaultSize, wxCAPTION | wxCLOSE_BOX)
    {
        static const std::vector<std::string> full_list = {"0.2", "0.4", "0.6", "0.8"};
        std::set<std::string> returned_set(diameters_raw.begin(), diameters_raw.end());
        std::vector<bool> item_enabled(full_list.size(), true);
        bool any_enabled = false;
        for (size_t i = 0; i < full_list.size(); ++i) {
            bool in = (returned_set.count(full_list[i]) > 0);
            item_enabled[i] = in;
            if (in) any_enabled = true;
        }
        if (!any_enabled)
            item_enabled.assign(full_list.size(), true);
        m_diameters = full_list;

        SetBackgroundColour(*wxWHITE);
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        wxStaticText* msg = new wxStaticText(this, wxID_ANY, message);
        msg->Wrap(FromDIP(400));
        sizer->Add(msg, 0, wxALL, FromDIP(10));
        std::vector<wxString> labels;
        for (const auto& d : m_diameters)
            labels.push_back(_L("Nozzle") + ": " + from_u8(d) + "mm");
        m_radio = new RadioGroup(this, labels, wxHORIZONTAL, 2);
        for (size_t i = 0; i < item_enabled.size(); ++i)
            if (!item_enabled[i])
                m_radio->SetItemEnabled((int)i, false);
        int first = 0;
        for (; first < (int)item_enabled.size(); ++first)
            if (item_enabled[first]) break;
        if (first >= (int)item_enabled.size()) first = 0;
        m_radio->SetSelection(first, false);
        sizer->Add(m_radio, 0, wxALL, FromDIP(10));
        auto* btns = new DialogButtons(this, {"OK", "Cancel"});
        btns->GetOK()->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { EndModal(wxID_OK); });
        btns->GetCANCEL()->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { EndModal(wxID_CANCEL); });
        sizer->Add(btns, 0, wxEXPAND);
        SetSizer(sizer);
        Layout();
        Fit();
        Centre(wxBOTH);
        wxGetApp().UpdateDlgDarkUI(this);
    }
    int GetSelection() const { return m_radio ? m_radio->GetSelection() : -1; }
    std::string GetSelectedDiameter() const {
        int idx = GetSelection();
        return (idx >= 0 && idx < (int)m_diameters.size()) ? m_diameters[idx] : std::string();
    }
    void on_dpi_changed(const wxRect& suggested_rect) override {}
};

std::string extract_base_filament_name(const std::string& full_name)
{
    std::string base = full_name;
    size_t at_pos = base.find('@');
    if (at_pos != std::string::npos) {
        base = base.substr(0, at_pos);
        base.erase(0, base.find_first_not_of(" \t\n\r"));
        base.erase(base.find_last_not_of(" \t\n\r") + 1);
    }
    return base;
}

// Resolve a machine filament name to a matching local filament preset.
// Filament presets follow the convention "BaseName @Model nozzle",
// e.g. "Generic PA-CF @U1 0.4 nozzle".  Split by '@' to extract the
// base name, trim, and compare exactly — so "Generic PA" does NOT
// accidentally match "Generic PA-CF".
Preset* resolve_filament_preset(PresetBundle* preset_bundle, 
    const std::string& filament_name, const std::string& filament_type, int spool_id)
{
    if (!preset_bundle || filament_name.empty())
        return nullptr;

    // paxx exposes the Spoolman spool ID as part of print_task_config. Prefer
    // the explicit Orca profile stored on that spool's filament over name
    // matching, which may otherwise fall back to a Generic profile.
    if (spool_id > 0) {
        std::string spoolman_url;
        const Preset& selected = preset_bundle->filaments.get_edited_preset();
        if (selected.config.has("spoolman_url"))
            spoolman_url = selected.config.opt_string("spoolman_url", 0u);
        if (spoolman_url.empty()) {
            for (const Preset& preset : preset_bundle->filaments) {
                if (preset.config.has("spoolman_url")) {
                    spoolman_url = preset.config.opt_string("spoolman_url", 0u);
                    if (!spoolman_url.empty())
                        break;
                }
            }
        }
        while (!spoolman_url.empty() && spoolman_url.back() == '/')
            spoolman_url.pop_back();

        if (!spoolman_url.empty()) {
            std::string response;
            std::string request_error;
            unsigned status = 0;
            auto http = Http::get(spoolman_url + "/api/v1/spool/" + std::to_string(spool_id));
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
                    const nlohmann::json spool = nlohmann::json::parse(response);
                    const nlohmann::json& filament = spool.at("filament");
                    const nlohmann::json& extra = filament.at("extra");
                    if (extra.contains("orca_profile") && extra["orca_profile"].is_string()) {
                        std::string profile_name = extra["orca_profile"].get<std::string>();
                        try {
                            const nlohmann::json decoded = nlohmann::json::parse(profile_name);
                            if (decoded.is_string())
                                profile_name = decoded.get<std::string>();
                        } catch (...) {
                            // Older/manual entries may already contain plain text.
                        }
                        if (Preset* preset = preset_bundle->filaments.find_preset(profile_name, false, true)) {
                            if (preset->is_compatible) {
                                BOOST_LOG_TRIVIAL(info) << "Spoolman profile resolved: spool=" << spool_id
                                                        << " profile=" << profile_name;
                                return preset;
                            }
                            BOOST_LOG_TRIVIAL(warning) << "Spoolman profile is not compatible with current printer: "
                                                       << profile_name;
                        } else {
                            BOOST_LOG_TRIVIAL(warning) << "Spoolman profile is not installed in Orca: " << profile_name;
                        }
                    }
                } catch (const std::exception& e) {
                    BOOST_LOG_TRIVIAL(warning) << "Invalid Spoolman response for spool " << spool_id << ": " << e.what();
                }
            } else {
                BOOST_LOG_TRIVIAL(warning) << "Could not load Spoolman spool " << spool_id
                                           << " (HTTP " << status << "): " << request_error;
            }
        }
    }

    auto to_lower = [](std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    };

    for (auto& preset : preset_bundle->filaments) {
        if (!preset.is_compatible)
            continue;

        std::string base = extract_base_filament_name(preset.name);
        if (to_lower(base) == to_lower(filament_name)) {
            return &preset;
        }
    }

    const std::string generic_prefix = "Generic ";
    std::string generic_base = generic_prefix + filament_type;
    for (auto& preset : preset_bundle->filaments) {
        if (!preset.is_compatible || !preset.is_system)
            continue;

        std::string base = extract_base_filament_name(preset.name);
        std::string type = preset.config.opt_string("filament_type", static_cast<unsigned int>(0));

        if ((to_lower(type) == to_lower(filament_type)) && 
                (to_lower(base) == to_lower(generic_base))) {
            return &preset;
        }
    }

    return nullptr;
}

void build_design_filament_list(PresetBundle* preset_bundle, std::vector<FilamentData>& out_list)
{
    if (!preset_bundle)
        return;

    const auto& filament_presets = preset_bundle->filament_presets;
    const auto* colors_opt = preset_bundle->project_config.option<ConfigOptionStrings>("filament_colour");
    const auto* multiColorsOpt = preset_bundle->project_config.option<ConfigOptionStrings>("filament_multi_colors");
    const auto* colorModeOpt   = preset_bundle->project_config.option<ConfigOptionInts>("filament_colour_mode");

    for (size_t i = 0; i < filament_presets.size(); ++i) {
        FilamentData fd;
        fd.m_index = i;

        Preset* preset = preset_bundle->filaments.find_preset(filament_presets[i]);
         if (preset) {
            fd.m_name = preset->label(false);
            const auto* type_opt = preset->config.option<ConfigOptionStrings>("filament_type");
            if (type_opt && !type_opt->values.empty())
                fd.m_type = type_opt->values[0];
        } else {
            fd.m_name = extract_base_filament_name(filament_presets[i]);
        }

        if (fd.m_type.empty()) {
            const auto* type_opt = preset_bundle->project_config.option<ConfigOptionStrings>("filament_type");
            if (type_opt && i < type_opt->values.size())
                fd.m_type = type_opt->values[i];
        }

        {
            std::vector<std::string> filamentColors;
            FilamentColorMode mode = FilamentColorMode::Segment;

            if (multiColorsOpt && i < multiColorsOpt->values.size() && !multiColorsOpt->values[i].empty())
                filamentColors = SplitFilamentMultiColors(multiColorsOpt->values[i]);
            else if (colors_opt && i < colors_opt->values.size())
                filamentColors = {colors_opt->values[i]};

            if (colorModeOpt && i < colorModeOpt->values.size())
                mode = FilamentColorModeFromConfig(colorModeOpt->values[i]);

            fd.m_color = FilamentColor::FromColors(filamentColors, mode);
        }

        out_list.push_back(std::move(fd));
    }
}

void build_machine_filament_list(PresetBundle* preset_bundle, std::vector<FilamentData>& out_list)
{
    if (!preset_bundle)
        return;

    for (const auto& info : preset_bundle->m_connect_machine_info_list) {
        FilamentData fd;
        fd.m_index = info.index;
        fd.m_name  = info.filament_info;
        fd.m_type  = info.filament_type;
        fd.m_spool_id = info.spool_id;
        
        if (!info.color_info.empty() || !info.multiColors.empty()) {
            fd.m_color = FilamentColor::FromColors(info.multiColors, info.colorMode, info.color_info);
        }

        out_list.push_back(std::move(fd));
    }
}

} // namespace

bool Plater::has_illegal_filename_characters(const wxString& wxs_name)
{
    std::string name = into_u8(wxs_name);
    return has_illegal_filename_characters(name);
}

bool Plater::has_illegal_filename_characters(const std::string& name)
{
    const char* illegal_characters = "<>:/\\|?*\"";
    for (size_t i = 0; i < std::strlen(illegal_characters); i++)
        if (name.find_first_of(illegal_characters[i]) != std::string::npos)
            return true;

    return false;
}

void Plater::show_illegal_characters_warning(wxWindow* parent)
{
    show_error(parent, _L("Invalid name, the following characters are not allowed:") + " <>:/\\|?*\"");
}

enum SlicedInfoIdx
{
    siFilament_m,
    siFilament_mm3,
    siFilament_g,
    siMateril_unit,
    siCost,
    siEstimatedTime,
    siWTNumbetOfToolchanges,
    siCount
};

enum class LoadFilesType {
    NoFile,
    Single3MF,
    SingleOther,
    Multiple3MF,
    MultipleOther,
    Multiple3MFOther,
};

enum class LoadType : unsigned char
{
    Unknown,
    OpenProject,
    LoadGeometry,
    LoadConfig
};

class SlicedInfo : public wxStaticBoxSizer
{
public:
    SlicedInfo(wxWindow *parent);
    void SetTextAndShow(SlicedInfoIdx idx, const wxString& text, const wxString& new_label="");

private:
    std::vector<std::pair<wxStaticText*, wxStaticText*>> info_vec;
};

SlicedInfo::SlicedInfo(wxWindow *parent) :
    wxStaticBoxSizer(new wxStaticBox(parent, wxID_ANY, _L("Sliced Info")), wxVERTICAL)
{
    GetStaticBox()->SetFont(wxGetApp().bold_font());
    wxGetApp().UpdateDarkUI(GetStaticBox());

    auto *grid_sizer = new wxFlexGridSizer(2, 5, 15);
    grid_sizer->SetFlexibleDirection(wxVERTICAL);

    info_vec.reserve(siCount);

    auto init_info_label = [this, parent, grid_sizer](wxString text_label) {
        auto *text = new wxStaticText(parent, wxID_ANY, text_label);
        text->SetForegroundColour(*wxBLACK);
        text->SetFont(wxGetApp().small_font());
        auto info_label = new wxStaticText(parent, wxID_ANY, "N/A");
        info_label->SetForegroundColour(*wxBLACK);
        info_label->SetFont(wxGetApp().small_font());
        grid_sizer->Add(text, 0);
        grid_sizer->Add(info_label, 0);
        info_vec.push_back(std::pair<wxStaticText*, wxStaticText*>(text, info_label));
    };

    init_info_label(_L("Used Filament (m)"));
    init_info_label(_L("Used Filament (mm³)"));
    init_info_label(_L("Used Filament (g)"));
    init_info_label(_L("Used Materials"));
    init_info_label(_L("Cost"));
    init_info_label(_L("Estimated time"));
    init_info_label(_L("Filament changes"));

    Add(grid_sizer, 0, wxEXPAND);
    this->Show(false);
}

void SlicedInfo::SetTextAndShow(SlicedInfoIdx idx, const wxString& text, const wxString& new_label/*=""*/)
{
    const bool show = text != "N/A";
    if (show)
        info_vec[idx].second->SetLabelText(text);
    if (!new_label.IsEmpty())
        info_vec[idx].first->SetLabelText(new_label);
    info_vec[idx].first->Show(show);
    info_vec[idx].second->Show(show);
}

static wxString temp_dir;

// Sidebar / private

enum class ActionButtonType : int {
    abReslice,
    abExport,
    abSendGCode
};

int SidebarProps::TitlebarMargin() { return 8; }  // Use as side margins on titlebar. Has less margin on sides to create separation with its content
int SidebarProps::ContentMargin()  { return 12; } // Use as side margins contents of title
int SidebarProps::IconSpacing()    { return 10; } // Use on main elements
int SidebarProps::ElementSpacing() { return 5; }  // Use if elements has relation between them like edit button for combo box etc.
// CustomNotebook.h
#pragma once

#include <wx/wx.h>
#include <vector>

class CustomNotebook : public wxControl
{
public:
    CustomNotebook(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize)
        : wxControl(parent, id, pos, size, wxBORDER_NONE), m_selectedIndex(-1), m_tabHeight(24), m_tabPadding(10), m_roundRadius(5)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        UpdateColors();

        Bind(wxEVT_PAINT, &CustomNotebook::OnPaint, this);
        Bind(wxEVT_ERASE_BACKGROUND, &CustomNotebook::OnEraseBackground, this);
        Bind(wxEVT_LEFT_DOWN, &CustomNotebook::OnLeftDown, this);
        Bind(wxEVT_SIZE, &CustomNotebook::OnSize, this);
    }

    void AddPage(wxWindow* page, const wxString& text)
    {
        m_tabs.push_back({text, page});
        if (page) {
            page->Reparent(this);
            page->Hide();
            page->SetBackgroundColour(m_selectedTabColor);
        }

        if (m_selectedIndex == -1) {
            SetSelection(0);
        }

        UpdateLayout();
        Refresh();
    }

    void DeleteAllPages()
    {
        for (auto& tab : m_tabs) {
            if (tab.page) {
                tab.page->Destroy();
            }
        }
        m_tabs.clear();
        m_selectedIndex = -1;
        UpdateLayout();
        Refresh();
    }

    size_t GetPageCount() const { return m_tabs.size(); }

    wxWindow* GetPage(size_t index) const { return (index < m_tabs.size()) ? m_tabs[index].page : nullptr; }

    int GetSelection() const { return m_selectedIndex; }

    void SetSelection(size_t index)
    {
        if (index >= m_tabs.size() || static_cast<int>(index) == m_selectedIndex)
            return;

        if (m_selectedIndex != -1 && m_tabs[m_selectedIndex].page) {
            m_tabs[m_selectedIndex].page->Hide();
        }

        m_selectedIndex = index;

        if (m_selectedIndex != -1 && m_tabs[m_selectedIndex].page) {
            m_tabs[m_selectedIndex].page->Show();
        }

        UpdateLayout();
        Refresh();
    }

protected:
    void OnPaint(wxPaintEvent& event)
    {
        UpdateColors();

        wxPaintDC dc(this);

        // 1. 绘制背景
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(m_bgColor));
        dc.DrawRectangle(GetClientRect());

        // 2. 绘制标签背景区域
        dc.SetPen(wxPen(m_dividerColor, 1));
        dc.SetBrush(wxBrush(m_dividerColor));
        wxRect labelRect(0, 0, GetSize().x, m_tabHeight);
        dc.DrawRoundedRectangle(labelRect, m_roundRadius);
        dc.DrawRectangle(0, m_tabHeight - 2, GetSize().x, 4);

        // 3. 绘制所有标签
        wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        font.SetPointSize(m_textSize);
        dc.SetFont(font);

        auto height = dc.GetCharHeight();
        if (height > m_tabHeight - 2) {
            m_tabHeight = height + 2;
            Layout();
        }

        int xPos = 0;
        for (size_t i = 0; i < m_tabs.size(); ++i) {
            bool isSelected = static_cast<int>(i) == m_selectedIndex;

            int textWidth, textHeight;
            dc.GetTextExtent(m_tabs[i].text, &textWidth, &textHeight);
            int tabWidth = textWidth + 2 * m_tabPadding;

            if (isSelected) {
                dc.SetPen(wxPen(m_dividerColor, 1));
                dc.SetBrush(wxBrush(m_bgColor));
                wxRect selectedRect(xPos, 0, tabWidth, m_tabHeight + 2);
                dc.DrawRectangle(selectedRect);
                dc.DrawRoundedRectangle(selectedRect, m_roundRadius);

                dc.SetPen(wxPen(m_bgColor, 1));
                dc.SetBrush(wxBrush(m_bgColor));
                dc.DrawRectangle(xPos, m_tabHeight, tabWidth, 4);
            }

            dc.SetTextForeground(isSelected ? m_selectedTextColor : m_textColor);
            dc.DrawText(m_tabs[i].text, xPos + m_tabPadding, (m_tabHeight - textHeight) / 2);

            xPos += tabWidth;
        }

        // 4. 绘制外边框
        dc.SetPen(wxPen(m_borderColor, 1));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRoundedRectangle(GetClientRect(), m_roundRadius);
    }

    void OnLeftDown(wxMouseEvent& event)
    {
        wxPoint pos = event.GetPosition();
        if (pos.y > m_tabHeight) {
            event.Skip();
            return;
        }

        int tabIndex = HitTest(pos);
        if (tabIndex != -1 && tabIndex != m_selectedIndex) {
            SetSelection(tabIndex);
            Refresh();
        }
    }

    void OnSize(wxSizeEvent& event)
    {
        UpdateLayout();
        Refresh();
        event.Skip();
    }

    void OnEraseBackground(wxEraseEvent& event) {}

private:
    struct TabInfo
    {
        wxString  text;
        wxWindow* page;
    };

    wxRect GetTabRect(size_t index) const
    {
        if (index >= m_tabs.size())
            return wxRect();

        wxClientDC dc(const_cast<CustomNotebook*>(this));
        wxFont     font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        font.SetPointSize(m_textSize);
        dc.SetFont(font);

        int textWidth, textHeight;
        dc.GetTextExtent(m_tabs[index].text, &textWidth, &textHeight);
        int tabWidth = textWidth + 2 * m_tabPadding;

        int x = 0;
        for (size_t i = 0; i < index; ++i) {
            dc.GetTextExtent(m_tabs[i].text, &textWidth, &textHeight);
            x += textWidth + 2 * m_tabPadding;
        }

        return wxRect(x, 0, tabWidth, m_tabHeight);
    }

    int HitTest(const wxPoint& pt) const
    {
        if (pt.y > m_tabHeight)
            return -1;

        wxClientDC dc(const_cast<CustomNotebook*>(this));
        wxFont     font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        font.SetPointSize(m_textSize);
        dc.SetFont(font);

        int xPos = 0;
        for (size_t i = 0; i < m_tabs.size(); ++i) {
            int textWidth, textHeight;
            dc.GetTextExtent(m_tabs[i].text, &textWidth, &textHeight);
            int tabWidth = textWidth + 2 * m_tabPadding;

            if (pt.x >= xPos && pt.x <= xPos + tabWidth) {
                return i;
            }

            xPos += tabWidth;
        }

        return -1;
    }

    void UpdateColors()
    {
        bool is_dark = wxGetApp().app_config->get("dark_color_mode") == "1";

        if (!is_dark) {
            m_bgColor           = wxColour(255, 255, 255);
            m_borderColor       = wxColour(240, 240, 240);
            m_selectedTabColor  = wxColour(255, 255, 255);
            m_textColor         = wxColour(194, 194, 193);
            m_dividerColor      = wxColour(240, 240, 240);
            m_selectedTextColor = wxColour(0, 0, 0);
        } else {
            m_bgColor           = wxColour(45, 45, 49);
            m_borderColor       = wxColour(76, 76, 85);
            m_selectedTabColor  = wxColour(45, 45, 49);
            m_textColor         = wxColour(104, 105, 107);
            m_dividerColor      = wxColour(51, 51, 55);
            m_selectedTextColor = wxColour(255, 255, 255);
        }
    }

    void UpdateLayout()
    {
        if (m_selectedIndex != -1 && m_tabs[m_selectedIndex].page) {
            wxSize size = GetSize();
            m_tabs[m_selectedIndex].page->SetSize(2, m_tabHeight + 1, size.x - 4, size.y - m_tabHeight - 4);
            m_tabs[m_selectedIndex].page->Layout();
        }
    }

private:
    std::vector<TabInfo> m_tabs;
    int                  m_selectedIndex;

    wxColour m_bgColor;
    wxColour m_borderColor;
    wxColour m_selectedTabColor;
    wxColour m_textColor;
    wxColour m_selectedTextColor;
    wxColour m_dividerColor;

    int m_tabHeight;
    int m_tabPadding;
    int m_roundRadius;
#ifdef _WIN32
    int m_textSize = 10;
#else
    int m_textSize = 13;
#endif
};

struct Sidebar::priv
{
    Plater *plater;

    wxPanel *scrolled;
    PlaterPresetComboBox *combo_print;
    std::vector<PlaterPresetComboBox*> combos_filament;
    int editing_filament = -1;
    wxBoxSizer *sizer_filaments;
    PlaterPresetComboBox *combo_sla_print;
    PlaterPresetComboBox *combo_sla_material;
    PlaterPresetComboBox* combo_printer = nullptr;
    wxBoxSizer *sizer_params;

    // test
    wxStaticBitmap * image_printer = nullptr;
    StaticBox*      panel_printer_preset = nullptr;
    

    //BBS Sidebar widgets
    wxPanel* m_panel_print_title;
    wxStaticText* m_staticText_print_title;
    wxPanel* m_panel_print_content;
    wxComboBox* m_comboBox_print_preset;
    wxStaticLine* m_staticline1;
    StaticBox* m_panel_filament_title;
    ScalableButton* m_filament_config_icon = nullptr;
    wxStaticText* m_staticText_filament_settings;
    ScalableButton *  m_bpButton_add_filament;
    ScalableButton *  m_bpButton_del_filament;
    ScalableButton *  m_bpButton_ams_filament;
    ScalableButton *  m_bpButton_set_filament;
    ScalableButton *  m_bpButton_sync_filament = nullptr;
    int                         m_menu_filament_id = -1;
    wxPanel* m_panel_filament_content;
    wxScrolledWindow* m_scrolledWindow_filament_content;

    // Mixed (virtual) filaments panel - collapsible like Printer/Filament sections
    StaticBox*          m_panel_mixed_filaments_title = nullptr;    // Collapsible title bar
    wxPanel*            m_panel_mixed_filaments_content = nullptr; // Content panel
    wxBoxSizer*         m_sizer_mixed_filaments_content = nullptr; // Content sizer
    ScalableButton*     m_mixed_filaments_icon = nullptr;          // Icon
    wxStaticText*       m_staticText_mixed_filaments = nullptr;    // Title text
    Button*             m_btn_add_gradient = nullptr;              // Add gradient button
    Button*             m_btn_add_pattern = nullptr;               // Add pattern button
    Button*             m_btn_add_color = nullptr;                 // Add color-match button
    Button*             m_btn_toggle_mixed_filaments = nullptr;   // Collapse/expand toggle button
    bool                m_mixed_filaments_collapsed = false;      // Collapse state
    bool                m_skip_mixed_filament_sync_once = false;  // Local edits already mutated manager in place.
    std::unordered_set<size_t> m_expanded_mixed_filament_rows;    // Expanded row editors
    struct MixedFilamentRowBinding {
        size_t    mixed_id = size_t(-1);
        wxWindow *row      = nullptr;
    };
    std::vector<MixedFilamentRowBinding> m_mixed_filament_row_bindings;
    std::vector<uint64_t>                m_mixed_filament_ui_order;
    bool                                 m_mixed_filament_drag_active = false;
    size_t                               m_mixed_filament_drag_source_mixed_id = size_t(-1);
    // Physical filament scrolled window
    wxScrolledWindow* m_scrolled_filaments = nullptr;
    wxPanel*          m_panel_scrolled_filament_content = nullptr;
    // Color mix panel
    StaticBox*      m_panel_physical_filaments_title = nullptr;
    StaticBox*      m_panel_color_mix_title   = nullptr;
    wxPanel*        m_panel_color_mix_content = nullptr;
    wxScrolledWindow* m_scrolled_color_mix    = nullptr;
    ScalableButton* m_color_mix_icon          = nullptr;
    ScalableButton* m_btn_add_color_mix       = nullptr;
    ScalableButton* m_btn_del_color_mix       = nullptr;

    wxStaticLine* m_staticline2;
    wxPanel* m_panel_project_title;
    ScalableButton* m_filament_icon = nullptr;
    Button * m_flushing_volume_btn = nullptr;
    Button*  m_btn_batch_match      = nullptr;              // Batch color-match mapping
    TextInput* m_search_item = nullptr;
    StaticBox* m_search_bar = nullptr;
    Search::SearchObjectDialog* dia = nullptr;

    // BBS printer config
    StaticBox* m_panel_printer_title = nullptr;
    ScalableButton* m_printer_icon = nullptr;
    ScalableButton* m_printerinfo_syncbtn = nullptr;
    ScalableButton* m_printer_setting = nullptr;
    wxStaticText* m_text_printer_settings = nullptr;
    wxPanel* m_panel_printer_content = nullptr;

    // nozzle notebook  and related controls
    CustomNotebook*                  m_nozzle_notebook{nullptr};
    std::vector<ComboBox*>       m_nozzle_diameter_lists;
    std::vector<ScalableButton*> m_nozzle_edit_btns;

    ObjectList          *m_object_list{ nullptr };
    ObjectSettings      *object_settings{ nullptr };
    ObjectLayers        *object_layers{ nullptr };

    wxButton *btn_export_gcode;
    wxButton *btn_reslice;
    ScalableButton *btn_send_gcode;
    //ScalableButton *btn_eject_device;
    ScalableButton* btn_export_gcode_removable; //exports to removable drives (appears only if removable drive is connected)

    Search::OptionsSearcher     searcher;
    std::string ams_list_device;

    priv(Plater *plater) : plater(plater) {}
    ~priv();

    void show_preset_comboboxes();
    void jump_to_object(ObjectDataViewModelNode* item);
    void can_search();

#ifdef _WIN32
    wxString btn_reslice_tip;
    void show_rich_tip(const wxString& tooltip, wxButton* btn);
    void hide_rich_tip(wxButton* btn);
#endif
};

Sidebar::priv::~priv()
{
    // BBS
    //delete object_manipulation;
    delete object_settings;
    // BBS
#if 0
    delete frequently_changed_parameters;
#endif
}

void Sidebar::priv::show_preset_comboboxes()
{
    const bool showSLA = wxGetApp().preset_bundle->printers.get_edited_preset().printer_technology() == ptSLA;

//BBS
#if 0
    for (size_t i = 0; i < 4; ++i)
        sizer_presets->Show(i, !showSLA);

    for (size_t i = 4; i < 8; ++i) {
        if (sizer_presets->IsShown(i) != showSLA)
            sizer_presets->Show(i, showSLA);
    }

    frequently_changed_parameters->Show(!showSLA);
#endif

    scrolled->GetParent()->Layout();
    scrolled->Refresh();
}

void Sidebar::priv::jump_to_object(ObjectDataViewModelNode* item)
{
    m_object_list->selected_object(item);
}

void Sidebar::priv::can_search()
{
    if (m_search_bar->IsShown()) {
        m_search_item->SetFocus();
    }
}

#ifdef _WIN32
using wxRichToolTipPopup = wxCustomBackgroundWindow<wxPopupTransientWindow>;
static wxRichToolTipPopup* get_rtt_popup(wxButton* btn)
{
    auto children = btn->GetChildren();
    for (auto child : children)
        if (child->IsShown())
            return dynamic_cast<wxRichToolTipPopup*>(child);
    return nullptr;
}

void Sidebar::priv::show_rich_tip(const wxString& tooltip, wxButton* btn)
{
    if (tooltip.IsEmpty())
        return;
    wxRichToolTip tip(tooltip, "");
    tip.SetIcon(wxICON_NONE);
    tip.SetTipKind(wxTipKind_BottomRight);
    tip.SetTitleFont(wxGetApp().normal_font());
    tip.SetBackgroundColour(wxGetApp().get_window_default_clr());

    tip.ShowFor(btn);
    // Every call of the ShowFor() creates new RichToolTip and show it.
    // Every one else are hidden.
    // So, set a text color just for the shown rich tooltip
    if (wxRichToolTipPopup* popup = get_rtt_popup(btn)) {
        auto children = popup->GetChildren();
        for (auto child : children) {
            child->SetForegroundColour(wxGetApp().get_label_clr_default());
            // we neen just first text line for out rich tooltip
            return;
        }
    }
}

void Sidebar::priv::hide_rich_tip(wxButton* btn)
{
    if (wxRichToolTipPopup* popup = get_rtt_popup(btn))
        popup->Dismiss();
}
#endif

std::vector<int> get_min_flush_volumes(const DynamicPrintConfig& full_config)
{
    std::vector<int>extra_flush_volumes;
    //const auto& full_config = wxGetApp().preset_bundle->full_config();
    //auto& printer_config = wxGetApp().preset_bundle->printers.get_edited_preset().config;

    const ConfigOption* nozzle_volume_opt = full_config.option("nozzle_volume");
    int nozzle_volume_val = nozzle_volume_opt ? (int)nozzle_volume_opt->getFloat() : 0;

    const ConfigOptionInt* enable_long_retraction_when_cut_opt = full_config.option<ConfigOptionInt>("enable_long_retraction_when_cut");
    int machine_enabled_level = 0;
    if (enable_long_retraction_when_cut_opt) {
        machine_enabled_level = enable_long_retraction_when_cut_opt->value;
        BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": get enable_long_retraction_when_cut from config, value=%1%")%machine_enabled_level;
    }
    const ConfigOptionBools* long_retractions_when_cut_opt = full_config.option<ConfigOptionBools>("long_retractions_when_cut");
    bool machine_activated = false;
    if (long_retractions_when_cut_opt) {
        machine_activated = long_retractions_when_cut_opt->values[0] == 1;
        BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": get long_retractions_when_cut from config, value=%1%, activated=%2%")%long_retractions_when_cut_opt->values[0] %machine_activated;
    }

    size_t filament_size = full_config.option<ConfigOptionFloats>("filament_diameter")->values.size();
    std::vector<double> filament_retraction_distance_when_cut(filament_size, 18.0f), printer_retraction_distance_when_cut(filament_size, 18.0f);
    std::vector<unsigned char> filament_long_retractions_when_cut(filament_size, 0);
    const ConfigOptionFloats* filament_retraction_distances_when_cut_opt = full_config.option<ConfigOptionFloats>("filament_retraction_distances_when_cut");
    if (filament_retraction_distances_when_cut_opt) {
        filament_retraction_distance_when_cut = filament_retraction_distances_when_cut_opt->values;
        BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": get filament_retraction_distance_when_cut from config, size=%1%, values=%2%")%filament_retraction_distance_when_cut.size() %filament_retraction_distances_when_cut_opt->serialize();
    }

    const ConfigOptionFloats* printer_retraction_distance_when_cut_opt = full_config.option<ConfigOptionFloats>("retraction_distances_when_cut");
    if (printer_retraction_distance_when_cut_opt) {
        printer_retraction_distance_when_cut = printer_retraction_distance_when_cut_opt->values;
        BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": get retraction_distances_when_cut from config, size=%1%, values=%2%")%printer_retraction_distance_when_cut.size() %printer_retraction_distance_when_cut_opt->serialize();
    }

    const ConfigOptionBools* filament_long_retractions_when_cut_opt = full_config.option<ConfigOptionBools>("filament_long_retractions_when_cut");
    if (filament_long_retractions_when_cut_opt) {
        filament_long_retractions_when_cut = filament_long_retractions_when_cut_opt->values;
        BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": get filament_long_retractions_when_cut from config, size=%1%, values=%2%")%filament_long_retractions_when_cut.size() %filament_long_retractions_when_cut_opt->serialize();
    }

    for (size_t idx = 0; idx < filament_size; ++idx) {
        int extra_flush_volume = nozzle_volume_val;
        int retract_length = machine_enabled_level && machine_activated ? printer_retraction_distance_when_cut[0] : 0;

        unsigned char filament_activated = filament_long_retractions_when_cut[idx];
        double filament_retract_length = filament_retraction_distance_when_cut[idx];

        if (filament_activated == 0)
            retract_length = 0;
        else if (filament_activated == 1 && machine_enabled_level == LongRectrationLevel::EnableFilament) {
            if (!std::isnan(filament_retract_length))
                retract_length = (int)filament_retraction_distance_when_cut[idx];
            else
                retract_length = printer_retraction_distance_when_cut[0];
        }

        extra_flush_volume -= PI * 1.75 * 1.75 / 4 * retract_length;
        extra_flush_volumes.emplace_back(extra_flush_volume);
    }
    return extra_flush_volumes;
}


// Sidebar / public

struct DynamicFilamentList : DynamicList
{
    std::vector<std::pair<wxString, wxBitmap *>> items;

    void apply_on(Choice *c) override
    {
        if (items.empty())
            update(true);
        auto cb = dynamic_cast<ComboBox *>(c->window);
        auto n  = cb->GetSelection();
        cb->Clear();
        cb->Append(_L("Default"));
        for (auto i : items) {
            cb->Append(i.first, *i.second);
        }
        if (n < cb->GetCount())
            cb->SetSelection(n);
    }
    wxString get_value(int index) override
    {
        wxString str;
        str << index;
        return str;
    }
    int index_of(wxString value) override
    {
        long n = 0;
        return (value.ToLong(&n) && n <= items.size()) ? int(n) : -1;
    }
    void update(bool force = false)
    {
        items.clear();
        if (!force && m_choices.empty())
            return;
        auto icons = get_extruder_color_icons(true);
        auto presets = wxGetApp().preset_bundle->filament_presets;
        for (int i = 0; i < presets.size(); ++i) {
            wxString str;
            std::string type;
            wxGetApp().preset_bundle->filaments.find_preset(presets[i])->get_filament_type(type);
            str << type;
            items.push_back({str, icons[i]});
        }
        DynamicList::update();
    }
};

struct DynamicFilamentList1Based : DynamicFilamentList
{
    void apply_on(Choice *c) override
    {
        if (items.empty())
            update(true);
        auto cb = dynamic_cast<ComboBox *>(c->window);
        auto n  = cb->GetSelection();
        cb->Clear();
        for (auto i : items) {
            cb->Append(i.first, *i.second);
        }
        if (n < cb->GetCount())
            cb->SetSelection(n);
    }
    wxString get_value(int index) override
    {
        wxString str;
        str << index+1;
        return str;
    }
    int index_of(wxString value) override
    {
        long n = 0;
        if(!value.ToLong(&n))
            return -1;
        --n;
        return (n >= 0 && n <= items.size()) ? int(n) : -1;
    }
    void update(bool force = false)
    {
        items.clear();
        if (!force && m_choices.empty())
            return;
        auto icons = get_extruder_color_icons(true);
        auto presets = wxGetApp().preset_bundle->filament_presets;
        for (int i = 0; i < presets.size(); ++i) {
            wxString str;
            std::string type;
            wxGetApp().preset_bundle->filaments.find_preset(presets[i])->get_filament_type(type);
            str << type;
            items.push_back({str, icons[i]});
        }
        DynamicList::update();
    }

};

class MixedFilamentColorMatchDialog : public DPIDialog
{
public:
    MixedFilamentColorMatchDialog(wxWindow* parent, const std::vector<std::string>& physical_colors, const wxColour& initial_color)
        : DPIDialog(parent ? parent : static_cast<wxWindow*>(wxGetApp().mainframe),
                    wxID_ANY,
                    _L("Add Color"),
                    wxDefaultPosition,
                    wxDefaultSize,
                    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
        , m_physical_colors(physical_colors)
    {
        m_recipe_timer.SetOwner(this);
        m_loading_timer.SetOwner(this);
        m_display_context = build_mixed_filament_display_context(m_physical_colors);

        m_palette.reserve(m_physical_colors.size());
        for (const std::string& hex : m_physical_colors)
            m_palette.emplace_back(parse_mixed_color(hex));

        const wxColour   safe_initial = initial_color.IsOk() ?
                                            initial_color :
                                            (m_palette.size() >= 2 ? blend_pair_filament_mixer(m_palette[0], m_palette[1], 0.5f) :
                                                                     wxColour("#26A69A"));
        std::vector<int> initial_weights(m_palette.size(), 0);
        if (!initial_weights.empty())
            initial_weights[0] = 100;
        if (initial_weights.size() >= 2) {
            initial_weights[0] = 50;
            initial_weights[1] = 50;
        }

        std::vector<unsigned int> filament_ids;
        filament_ids.reserve(m_palette.size());
        for (size_t idx = 0; idx < m_palette.size(); ++idx)
            filament_ids.emplace_back(unsigned(idx + 1));

        SetMinSize(wxSize(FromDIP(430), FromDIP(520)));

        auto* root        = new wxBoxSizer(wxVERTICAL);
        auto* description = new wxStaticText(this, wxID_ANY,
                                             _L("Pick from the current filament gamut. The dialog previews the closest 2-color, 3-color, "
                                                "or 4-color FilamentMixer recipe before it is added."));
        description->Wrap(FromDIP(390));
        root->Add(description, 0, wxEXPAND | wxALL, FromDIP(12));

        m_color_map = new MixedFilamentColorMapPanel(this, filament_ids, m_palette, initial_weights, wxSize(FromDIP(260), FromDIP(260)));
        root->Add(m_color_map, 1, wxEXPAND | wxLEFT | wxRIGHT, FromDIP(12));

        auto* hex_row = new wxBoxSizer(wxHORIZONTAL);
        hex_row->Add(new wxStaticText(this, wxID_ANY, _L("Hex")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(8));
        m_hex_input = new wxTextCtrl(this, wxID_ANY, normalize_color_match_hex(safe_initial.GetAsString(wxC2S_HTML_SYNTAX)),
                                     wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
        m_hex_input->SetToolTip(_L("Enter a hex color like #00FF88. The picker will snap to the closest supported FilamentMixer color."));
        hex_row->Add(m_hex_input, 1, wxALIGN_CENTER_VERTICAL);
        hex_row->AddSpacer(FromDIP(8));
        m_classic_picker = new wxColourPickerCtrl(this, wxID_ANY, safe_initial);
        m_classic_picker->SetToolTip(_L("Classic color picker. The result will snap to the closest supported FilamentMixer color."));
        hex_row->Add(m_classic_picker, 0, wxALIGN_CENTER_VERTICAL);
        root->Add(hex_row, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, FromDIP(12));

        auto* range_row = new wxBoxSizer(wxHORIZONTAL);
        range_row->Add(new wxStaticText(this, wxID_ANY, _L("Range")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(8));
        m_range_slider = new wxSlider(this, wxID_ANY, m_min_component_percent, 0, 50);
        m_range_slider->SetToolTip(_L("Minimum percent for each participating color. Higher values block highly skewed mixes."));
        range_row->Add(m_range_slider, 1, wxALIGN_CENTER_VERTICAL);
        range_row->AddSpacer(FromDIP(8));
        m_range_value = new wxStaticText(this, wxID_ANY, wxEmptyString);
        range_row->Add(m_range_value, 0, wxALIGN_CENTER_VERTICAL);
        root->Add(range_row, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, FromDIP(12));

        auto* summary_grid = new wxFlexGridSizer(2, FromDIP(8), FromDIP(8));
        summary_grid->AddGrowableCol(1, 1);

        summary_grid->Add(new wxStaticText(this, wxID_ANY, _L("Requested")), 0, wxALIGN_CENTER_VERTICAL);
        auto* selected_row = new wxBoxSizer(wxHORIZONTAL);
        m_selected_preview = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(FromDIP(72), FromDIP(24)), wxBORDER_SIMPLE);
        selected_row->Add(m_selected_preview, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(8));
        m_selected_label = new wxStaticText(this, wxID_ANY, wxEmptyString);
        selected_row->Add(m_selected_label, 1, wxALIGN_CENTER_VERTICAL);
        summary_grid->Add(selected_row, 1, wxEXPAND);

        summary_grid->Add(new wxStaticText(this, wxID_ANY, _L("Creates")), 0, wxALIGN_CENTER_VERTICAL);
        auto* recipe_row = new wxBoxSizer(wxHORIZONTAL);
        m_recipe_preview = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(FromDIP(72), FromDIP(24)), wxBORDER_SIMPLE);
        recipe_row->Add(m_recipe_preview, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(8));
        m_recipe_label = new wxStaticText(this, wxID_ANY, wxEmptyString);
        m_recipe_label->Wrap(FromDIP(280));
        recipe_row->Add(m_recipe_label, 1, wxALIGN_CENTER_VERTICAL);
        summary_grid->Add(recipe_row, 1, wxEXPAND);

        root->Add(summary_grid, 0, wxEXPAND | wxALL, FromDIP(12));

        m_delta_label = new wxStaticText(this, wxID_ANY, wxEmptyString);
        root->Add(m_delta_label, 0, wxEXPAND | wxLEFT | wxRIGHT, FromDIP(12));

        m_presets_label = new wxStaticText(this, wxID_ANY, _L("Exact preset mixes"));
        root->Add(m_presets_label, 0, wxLEFT | wxRIGHT | wxTOP, FromDIP(12));
        m_presets_host = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxSize(-1, FromDIP(96)), wxVSCROLL | wxBORDER_SIMPLE);
        m_presets_host->SetScrollRate(FromDIP(6), FromDIP(6));
        m_presets_sizer = new wxWrapSizer(wxHORIZONTAL, wxWRAPSIZER_DEFAULT_FLAGS);
        m_presets_host->SetSizer(m_presets_sizer);
        root->Add(m_presets_host, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, FromDIP(12));

        m_error_label = new wxStaticText(this, wxID_ANY, wxEmptyString);
        m_error_label->SetForegroundColour(StateColor::darkModeColorFor(wxColour("#D32F2F")));
        root->Add(m_error_label, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, FromDIP(12));

        if (wxSizer* button_sizer = CreateStdDialogButtonSizer(wxOK | wxCANCEL))
            root->Add(button_sizer, 0, wxEXPAND | wxALL, FromDIP(12));

        m_loading_panel = new wxPanel(this, wxID_ANY);
        m_loading_panel->SetMinSize(wxSize(-1, FromDIP(24)));
        auto* loading_row = new wxBoxSizer(wxHORIZONTAL);
        m_loading_label   = new wxStaticText(m_loading_panel, wxID_ANY, " ");
        loading_row->Add(m_loading_label, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(8));
        m_loading_gauge = new wxGauge(m_loading_panel, wxID_ANY, 100, wxDefaultPosition, wxSize(FromDIP(150), FromDIP(8)),
                                      wxGA_HORIZONTAL | wxGA_SMOOTH);
        m_loading_gauge->SetValue(0);
        m_loading_gauge->Enable(false);
        loading_row->Add(m_loading_gauge, 0, wxALIGN_CENTER_VERTICAL);
        m_loading_panel->SetSizer(loading_row);
        root->Add(m_loading_panel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(12));

        SetSizerAndFit(root);

        m_selected_target  = safe_initial;
        m_requested_target = safe_initial;
        if (m_color_map)
            m_color_map->set_min_component_percent(m_min_component_percent);
        update_range_label();
        rebuild_presets_ui();
        sync_inputs_to_requested();
        update_dialog_state();

        if (m_color_map) {
            m_color_map->Bind(wxEVT_SLIDER, [this](wxCommandEvent&) {
                if (!m_color_map)
                    return;
                request_recipe_match(m_color_map->selected_color(), true, _L("Matching closest supported mix..."));
            });
        }

        if (m_hex_input) {
            m_hex_input->Bind(wxEVT_TEXT_ENTER, [this](wxCommandEvent&) { apply_hex_input(true); });
            m_hex_input->Bind(wxEVT_KILL_FOCUS, [this](wxFocusEvent& evt) {
                apply_hex_input(false);
                evt.Skip();
            });
        }
        if (m_classic_picker) {
            m_classic_picker->Bind(wxEVT_COLOURPICKER_CHANGED, [this](wxColourPickerEvent& evt) {
                if (m_syncing_inputs)
                    return;
                apply_requested_target(evt.GetColour());
            });
        }
        if (m_range_slider) {
            m_range_slider->Bind(wxEVT_SLIDER, [this](wxCommandEvent&) {
                m_min_component_percent = m_range_slider ? std::clamp(m_range_slider->GetValue(), 0, 50) : m_min_component_percent;
                update_range_label();
                if (m_color_map)
                    m_color_map->set_min_component_percent(m_min_component_percent);
                rebuild_presets_ui();
                request_recipe_match(m_requested_target, true, _L("Matching closest supported mix..."));
            });
        }

        Bind(wxEVT_TIMER, [this](wxTimerEvent&) { refresh_selected_recipe(); }, m_recipe_timer.GetId());
        Bind(
            wxEVT_TIMER,
            [this](wxTimerEvent&) {
                if (m_loading_gauge && m_recipe_loading)
                    m_loading_gauge->Pulse();
            },
            m_loading_timer.GetId());
        if (wxWindow* ok_button = FindWindow(wxID_OK)) {
            ok_button->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt) {
                if (m_recipe_refresh_pending)
                    refresh_selected_recipe();
                if (m_recipe_loading || !m_selected_recipe.valid)
                    return;
                evt.Skip();
            });
        }

        Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent&) { EndModal(wxID_CANCEL); });

        CentreOnParent();
        wxGetApp().UpdateDlgDarkUI(this);
    }

    ~MixedFilamentColorMatchDialog() override
    {
        if (m_recipe_timer.IsRunning())
            m_recipe_timer.Stop();
        if (m_loading_timer.IsRunning())
            m_loading_timer.Stop();
    }

    void begin_initial_recipe_load() { request_recipe_match(m_requested_target, false, _L("Calculating closest supported mix...")); }

    MixedColorMatchRecipeResult selected_recipe() const { return m_selected_recipe; }

    void on_dpi_changed(const wxRect& suggested_rect) override
    {
        wxUnusedVar(suggested_rect);
        Layout();
        Fit();
        Refresh();
    }

private:
    void sync_recipe_preview(MixedColorMatchRecipeResult& recipe, const wxColour* requested_target = nullptr)
    {
        if (!recipe.valid)
            return;

        recipe.preview_color = compute_color_match_recipe_display_color(recipe, m_display_context);
        if (requested_target != nullptr && requested_target->IsOk())
            recipe.delta_e = color_delta_e00(*requested_target, recipe.preview_color);
    }

    void update_range_label()
    {
        if (m_range_value)
            m_range_value->SetLabel(wxString::Format(_L("%d%% min"), m_min_component_percent));
    }

    void rebuild_presets_ui()
    {
        if (!m_presets_host || !m_presets_sizer || !m_presets_label)
            return;

        m_presets = build_color_match_presets(m_physical_colors, m_min_component_percent);
        for (MixedColorMatchRecipeResult& preset : m_presets)
            sync_recipe_preview(preset);

        m_presets_host->Freeze();
        while (m_presets_sizer->GetItemCount() > 0) {
            wxSizerItem* item   = m_presets_sizer->GetItem(size_t(0));
            wxWindow*    window = item ? item->GetWindow() : nullptr;
            m_presets_sizer->Remove(0);
            if (window)
                window->Destroy();
        }

        for (const MixedColorMatchRecipeResult& preset : m_presets) {
            auto*          button  = new wxBitmapButton(m_presets_host, wxID_ANY,
                                                        make_color_match_swatch_bitmap(preset.preview_color, wxSize(FromDIP(30), FromDIP(20))),
                                                        wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
            const wxString tooltip = from_u8(summarize_color_match_recipe(preset)) + "\n" +
                                     normalize_color_match_hex(preset.preview_color.GetAsString(wxC2S_HTML_SYNTAX));
            button->SetToolTip(tooltip);
            button->Bind(wxEVT_BUTTON, [this, preset](wxCommandEvent&) { apply_preset(preset); });
            m_presets_sizer->Add(button, 0, wxALL, FromDIP(2));
        }

        m_presets_host->FitInside();
        const bool show_presets = !m_presets.empty();
        m_presets_label->Show(show_presets);
        m_presets_host->Show(show_presets);
        m_presets_host->Thaw();
    }

    void set_recipe_loading(bool loading, const wxString& message)
    {
        m_recipe_loading = loading;
        if (!message.empty())
            m_loading_message = message;

        if (m_loading_label)
            m_loading_label->SetLabel(loading ? m_loading_message : wxString(" "));
        if (m_loading_gauge) {
            if (loading) {
                m_loading_gauge->Enable(true);
                m_loading_gauge->Pulse();
                if (!m_loading_timer.IsRunning())
                    m_loading_timer.Start(100);
            } else {
                if (m_loading_timer.IsRunning())
                    m_loading_timer.Stop();
                m_loading_gauge->SetValue(0);
                m_loading_gauge->Enable(false);
            }
        }
    }

    void sync_inputs_to_requested()
    {
        m_syncing_inputs = true;
        if (m_hex_input)
            m_hex_input->ChangeValue(normalize_color_match_hex(m_requested_target.GetAsString(wxC2S_HTML_SYNTAX)));
        if (m_classic_picker)
            m_classic_picker->SetColour(m_requested_target);
        m_syncing_inputs = false;
    }

    bool apply_requested_target(const wxColour& requested_target)
    {
        request_recipe_match(requested_target, false, _L("Matching closest supported mix..."));
        return true;
    }

    bool apply_hex_input(bool show_invalid_error)
    {
        if (!m_hex_input || m_syncing_inputs)
            return false;

        wxColour parsed;
        if (!try_parse_color_match_hex(m_hex_input->GetValue(), parsed)) {
            if (show_invalid_error && m_error_label)
                m_error_label->SetLabel(_L("Use a valid hex color like #00FF88."));
            return false;
        }

        return apply_requested_target(parsed);
    }

    void request_recipe_match(const wxColour& requested_target, bool debounce, const wxString& loading_message)
    {
        m_requested_target = requested_target;
        m_selected_target  = requested_target;
        sync_inputs_to_requested();

        ++m_recipe_request_token;
        set_recipe_loading(true, loading_message);

        if (m_recipe_timer.IsRunning())
            m_recipe_timer.Stop();
        m_recipe_refresh_pending = debounce;
        update_dialog_state();

        if (debounce) {
            m_recipe_timer.StartOnce(120);
            return;
        }

        launch_recipe_match(m_recipe_request_token, requested_target);
    }

    void refresh_selected_recipe()
    {
        m_recipe_refresh_pending = false;
        launch_recipe_match(m_recipe_request_token, m_requested_target);
    }

    void launch_recipe_match(size_t request_token, const wxColour& requested_target)
    {
        const std::vector<std::string> physical_colors       = m_physical_colors;
        const int                      min_component_percent = m_min_component_percent;
        wxWeakRef<wxWindow>            weak_self(this);
        std::thread([weak_self, physical_colors, requested_target, request_token, min_component_percent]() {
            MixedColorMatchRecipeResult recipe = build_best_color_match_recipe(physical_colors, requested_target, min_component_percent);
            wxGetApp().CallAfter([weak_self, requested_target, recipe = std::move(recipe), request_token]() mutable {
                if (!weak_self)
                    return;
                auto* self = static_cast<MixedFilamentColorMatchDialog*>(weak_self.get());
                self->handle_recipe_result(request_token, requested_target, std::move(recipe));
            });
        }).detach();
    }

    void handle_recipe_result(size_t request_token, const wxColour& requested_target, MixedColorMatchRecipeResult recipe)
    {
        if (request_token != m_recipe_request_token)
            return;

        m_has_recipe_result = true;
        m_selected_recipe   = std::move(recipe);
        sync_recipe_preview(m_selected_recipe, &requested_target);
        set_recipe_loading(false, wxEmptyString);

        if (m_selected_recipe.valid) {
            m_selected_target = m_selected_recipe.preview_color;
            if (m_color_map)
                m_color_map->set_normalized_weights(expand_color_match_recipe_weights(m_selected_recipe, m_palette.size()), false);
            sync_inputs_to_requested();
        } else {
            m_selected_target = requested_target;
        }

        update_dialog_state();
    }

    void apply_preset(MixedColorMatchRecipeResult preset)
    {
        preset.delta_e = 0.0;
        sync_recipe_preview(preset);
        ++m_recipe_request_token;
        m_requested_target       = preset.preview_color;
        m_selected_target        = preset.preview_color;
        m_selected_recipe        = std::move(preset);
        m_has_recipe_result      = true;
        m_recipe_refresh_pending = false;
        if (m_recipe_timer.IsRunning())
            m_recipe_timer.Stop();
        set_recipe_loading(false, wxEmptyString);
        if (m_color_map)
            m_color_map->set_normalized_weights(expand_color_match_recipe_weights(m_selected_recipe, m_palette.size()), false);
        sync_inputs_to_requested();
        update_dialog_state();
    }

    void update_dialog_state()
    {
        const wxColour fallback = wxColour("#26A69A");
        if (m_selected_preview) {
            m_selected_preview->SetBackgroundColour(m_requested_target.IsOk() ? m_requested_target : fallback);
            m_selected_preview->Refresh();
        }
        if (m_selected_label)
            m_selected_label->SetLabel(m_requested_target.IsOk() ?
                                           normalize_color_match_hex(m_requested_target.GetAsString(wxC2S_HTML_SYNTAX)) :
                                           normalize_color_match_hex(fallback.GetAsString(wxC2S_HTML_SYNTAX)));

        const bool     valid        = m_selected_recipe.valid;
        const wxColour recipe_color = (valid && m_selected_recipe.preview_color.IsOk()) ?
                                          m_selected_recipe.preview_color :
                                          (m_requested_target.IsOk() ? m_requested_target : fallback);
        if (m_recipe_preview) {
            m_recipe_preview->SetBackgroundColour(recipe_color);
            m_recipe_preview->Refresh();
        }
        if (m_recipe_label) {
            if (m_recipe_loading) {
                m_recipe_label->SetLabel(m_loading_message);
            } else if (valid) {
                const wxString recipe_summary = from_u8(summarize_color_match_recipe(m_selected_recipe));
                const wxString recipe_hex     = normalize_color_match_hex(recipe_color.GetAsString(wxC2S_HTML_SYNTAX));
                m_recipe_label->SetLabel(recipe_summary + "  " + recipe_hex);
            } else if (m_has_recipe_result) {
                m_recipe_label->SetLabel(_L("No supported 2-color, 3-color, or 4-color recipe found."));
            } else {
                m_recipe_label->SetLabel(wxEmptyString);
            }
        }
        if (m_delta_label) {
            if (m_recipe_loading && m_requested_target.IsOk()) {
                m_delta_label->SetLabel(
                    wxString::Format(_L("Matching %s..."), normalize_color_match_hex(m_requested_target.GetAsString(wxC2S_HTML_SYNTAX))));
            } else if (valid && m_requested_target.IsOk()) {
                m_delta_label->SetLabel(wxString::Format(_L("Requested %s, closest recipe delta: %.2f"),
                                                         normalize_color_match_hex(m_requested_target.GetAsString(wxC2S_HTML_SYNTAX)),
                                                         m_selected_recipe.delta_e));
            } else {
                m_delta_label->SetLabel(wxEmptyString);
            }
        }
        if (m_error_label) {
            if (m_recipe_loading)
                m_error_label->SetLabel(wxEmptyString);
            else if (!valid && m_has_recipe_result)
                m_error_label->SetLabel(
                    _L("Unable to create a color mix from the current physical filament colors within the selected range."));
            else if (m_hex_input && !m_syncing_inputs) {
                wxColour parsed;
                if (!try_parse_color_match_hex(m_hex_input->GetValue(), parsed))
                    m_error_label->SetLabel(_L("Use a valid hex color like #00FF88."));
                else
                    m_error_label->SetLabel(wxEmptyString);
            } else {
                m_error_label->SetLabel(wxEmptyString);
            }
        }
        if (wxWindow* ok_button = FindWindow(wxID_OK))
            ok_button->Enable(valid && !m_recipe_loading && !m_recipe_refresh_pending);

        Layout();
    }

private:
    std::vector<std::string>                 m_physical_colors;
    MixedFilamentDisplayContext              m_display_context;
    std::vector<wxColour>                    m_palette;
    std::vector<MixedColorMatchRecipeResult> m_presets;
    MixedFilamentColorMapPanel*              m_color_map        = nullptr;
    wxTextCtrl*                              m_hex_input        = nullptr;
    wxColourPickerCtrl*                      m_classic_picker   = nullptr;
    wxSlider*                                m_range_slider     = nullptr;
    wxStaticText*                            m_range_value      = nullptr;
    wxStaticText*                            m_presets_label    = nullptr;
    wxScrolledWindow*                        m_presets_host     = nullptr;
    wxWrapSizer*                             m_presets_sizer    = nullptr;
    wxPanel*                                 m_loading_panel    = nullptr;
    wxStaticText*                            m_loading_label    = nullptr;
    wxGauge*                                 m_loading_gauge    = nullptr;
    wxPanel*                                 m_selected_preview = nullptr;
    wxStaticText*                            m_selected_label   = nullptr;
    wxPanel*                                 m_recipe_preview   = nullptr;
    wxStaticText*                            m_recipe_label     = nullptr;
    wxStaticText*                            m_delta_label      = nullptr;
    wxStaticText*                            m_error_label      = nullptr;
    wxColour                                 m_requested_target{wxColour("#26A69A")};
    wxColour                                 m_selected_target{wxColour("#26A69A")};
    MixedColorMatchRecipeResult              m_selected_recipe;
    wxTimer                                  m_recipe_timer;
    wxTimer                                  m_loading_timer;
    wxString                                 m_loading_message;
    size_t                                   m_recipe_request_token{0};
    int                                      m_min_component_percent{15};
    bool                                     m_has_recipe_result{false};
    bool                                     m_recipe_loading{false};
    bool                                     m_recipe_refresh_pending{false};
    bool                                     m_syncing_inputs{false};
};


MixedColorMatchRecipeResult prompt_best_color_match_recipe(wxWindow*                       parent,
                                                           const std::vector<std::string>& physical_colors,
                                                           const wxColour&                 initial_color)
{
    MixedFilamentColorMatchDialog dlg(parent, physical_colors, initial_color);
    dlg.begin_initial_recipe_load();
    if (dlg.ShowModal() != wxID_OK) {
        MixedColorMatchRecipeResult cancelled;
        cancelled.cancelled = true;
        return cancelled;
    }

    return dlg.selected_recipe();
}

static DynamicFilamentList dynamic_filament_list;
static DynamicFilamentList1Based dynamic_filament_list_1_based;

static wxString nozzle_type_key_to_label(const std::string& key)
{
    if (key == "hardened_steel")
        return _L("Hardened Steel");
    if (key == "stainless_steel")
        return _L("Stainless Steel");
    if (key == "brass")
        return _L("Brass");
    if (key == "undefine")
        return _L("Unknown");
    return wxString::FromUTF8(key);
}

Sidebar::Sidebar(Plater *parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(42 * wxGetApp().em_unit(), -1)), p(new priv(parent))
{
    Choice::register_dynamic_list("support_filament", &dynamic_filament_list);
    Choice::register_dynamic_list("support_interface_filament", &dynamic_filament_list);
    Choice::register_dynamic_list("wall_filament", &dynamic_filament_list_1_based);
    Choice::register_dynamic_list("sparse_infill_filament", &dynamic_filament_list_1_based);
    Choice::register_dynamic_list("solid_infill_filament", &dynamic_filament_list_1_based);
    Choice::register_dynamic_list("wipe_tower_filament", &dynamic_filament_list);

    p->scrolled = new wxPanel(this);
    p->scrolled->SetBackgroundColour(*wxWHITE);


    SetFont(wxGetApp().normal_font());
#ifndef __APPLE__
#ifdef _WIN32
    wxGetApp().UpdateDarkUI(this);
    wxGetApp().UpdateDarkUI(p->scrolled);
#else
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
#endif
#endif

    int em = wxGetApp().em_unit();
    //BBS refine layout and styles
    // Sizer in the scrolled area
    auto* scrolled_sizer = m_scrolled_sizer = new wxBoxSizer(wxVERTICAL);
    p->scrolled->SetSizer(scrolled_sizer);

    wxColour title_bg = wxColour(248, 248, 248);
    wxColour inactive_text = wxColour(86, 86, 86);
    wxColour active_text = wxColour(0, 0, 0);
    wxColour static_line_col = wxColour(166, 169, 170);

#ifdef __WINDOWS__
    p->scrolled->SetDoubleBuffered(true);
#endif //__WINDOWS__

    // add printer
    {
        /***************** 1. create printer title bar    **************/
        // 1.1 create title bar resources
        p->m_panel_printer_title = new StaticBox(p->scrolled, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_NONE);
        p->m_panel_printer_title->SetBackgroundColor(title_bg);
        p->m_panel_printer_title->SetBackgroundColor2(0xF1F1F1);

        p->m_printer_icon = new ScalableButton(p->m_panel_printer_title, wxID_ANY, "printer");
        p->m_text_printer_settings = new Label(p->m_panel_printer_title, _L("Printer"), LB_PROPAGATE_MOUSE_EVENT);

        // Use ams_fila_sync icon (sync_nozzle_info.svg does not exist in resources)
        p->m_printerinfo_syncbtn = new ScalableButton(p->m_panel_printer_title, wxID_ANY, "nozzle_sync");
        p->m_printerinfo_syncbtn->SetCursor(wxCURSOR_HAND);
        p->m_printerinfo_syncbtn->SetToolTip(_L("Synchronize nozzle information"));
        p->m_printerinfo_syncbtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent &e) {
            bool hasConnectDevice = false;
            auto devices = wxGetApp().app_config->get_devices();
            for (const auto& device : devices) {
                if (device.connected)
                    hasConnectDevice = true;
            }

            if (!hasConnectDevice)
            {
                // showdialog tips no connect device
                wxTheApp->CallAfter([this]() {
                    MessageDialog dlg(wxGetApp().mainframe,
                                      _L("Printer not connected. Please go to the home page or the device page to connect the printer."),
                                      _L("Note"), wxOK);
                    dlg.ShowModal();
                    });                
                return;        
            }

            std::string                machine_type = "";
            std::vector<std::string>   nozzle_diameters;
            std::string                device_name = "";
            std::shared_ptr<PrintHost> host = nullptr;
            wxGetApp().get_connect_host(host);
            const bool got_machine_info = SSWCP::query_machine_info(host, machine_type, nozzle_diameters, device_name);

            const auto& sync_nozzle_slots = wxGetApp().preset_bundle->m_connect_machine_info_list;
            if (!sync_nozzle_slots.empty()) {
                nozzle_diameters.clear();
                for (const auto& slot : sync_nozzle_slots) {
                    std::string nd = slot.nozzle_info;
                    boost::algorithm::trim(nd);
                    if (nd.size() > 2 && boost::iends_with(nd, "mm")) {
                        nd.resize(nd.size() - 2);
                        boost::algorithm::trim(nd);
                    }
                    if (!nd.empty())
                        nozzle_diameters.push_back(nd);
                }
            }
            if (got_machine_info && machine_type == "Snapmaker U1")
            {
                if (nozzle_diameters.size() <= 0)
                {
                    wxTheApp->CallAfter([this]() {
                        MessageDialog dlgEx(wxGetApp().mainframe,
                                            _L("No nozzle information detected. Please go to the printer settings to configure the nozzle."),
                                            _L("Note"), wxOK);
                        dlgEx.ShowModal();
                    });    

                    return;
                }

                bool res = false;
                std::string headNozzleSize = nozzle_diameters[0];
                for (int i = 1; i < nozzle_diameters.size(); i++)
                {
                    if (headNozzleSize != nozzle_diameters[i])
                    {
                        res = true;
                        break;
                    }
                }

                if (res)
                {
                    std::vector<std::string> diameters_raw = nozzle_diameters;
                    //std::vector<std::string> diameters_raw = {"0.2", "0.8"};
                    wxTheApp->CallAfter([this, diameters_raw]() {
                        NozzleDiameterSelectDialog dlg(
                            wxGetApp().mainframe,
                            _L("Note: Inconsistent nozzle diameters. Current version does not support mixed diameter printing. Please select one nozzle for this print."),
                            _L("Set Nozzle Diameter"),
                            diameters_raw);
                        if (dlg.ShowModal() == wxID_OK) {
                            std::string sel = dlg.GetSelectedDiameter();
                            if (!sel.empty()) {
                                auto preset = wxGetApp().preset_bundle->get_similar_printer_preset({}, sel);
                                if (preset) {
                                    preset->is_visible = true;

                                    auto diameter = sel;
                                    auto preset   = wxGetApp().preset_bundle->get_similar_printer_preset({}, diameter);
                                    if (preset == nullptr) {
                                        BOOST_LOG_TRIVIAL(error) << "get the similar printer preset fail";
                                        return;
                                    }
                                    preset->is_visible = true; // force visible

                                    for (size_t i = 0; i < p->m_nozzle_diameter_lists.size(); ++i) {
                                        p->m_nozzle_diameter_lists[i]->SetValue(diameter + "mm");
                                    }

                                    wxGetApp().get_tab(Preset::TYPE_PRINTER)->select_preset(preset->name);
                                    wxGetApp().plater()->sidebar().update_all_preset_comboboxes(true);
                                    wxGetApp().plater()->sidebar().update_nozzle_settings(true);
                                }
                            }
                        }
                    });
                    return;
                }
                else {
                    // All tool heads report the same diameter: apply it without opening the picker.
                    std::string diameter = headNozzleSize;
                    boost::algorithm::trim(diameter);
                    if (diameter.size() > 2 && boost::iends_with(diameter, "mm")) {
                        diameter.resize(diameter.size() - 2);
                        boost::algorithm::trim(diameter);
                    }
                    wxTheApp->CallAfter([this, diameter]() {
                        auto preset = wxGetApp().preset_bundle->get_similar_printer_preset({}, diameter);
                        if (preset == nullptr) {
                            BOOST_LOG_TRIVIAL(error) << "get the similar printer preset fail (uniform nozzle sync)";
                            return;
                        }
                        preset->is_visible = true;

                        for (size_t i = 0; i < p->m_nozzle_diameter_lists.size(); ++i)
                            p->m_nozzle_diameter_lists[i]->SetValue(diameter + "mm");

                        wxGetApp().get_tab(Preset::TYPE_PRINTER)->select_preset(preset->name);
                        wxGetApp().plater()->sidebar().update_all_preset_comboboxes(true);
                        wxGetApp().plater()->sidebar().update_nozzle_settings(true);

                        wxTheApp->CallAfter([this]() {
                            MessageDialog dlg_Ex(wxGetApp().mainframe, _L("Nozzle settings synchronized successfully"),
                                                 _L("Note"), wxOK);
                            dlg_Ex.ShowModal();
                        });
                    });
                }
            }
            
            });
        
        p->m_printer_setting = new ScalableButton(p->m_panel_printer_title, wxID_ANY, "settings");
        p->m_printer_setting->SetToolTip(_L("settings"));
        p->m_printer_setting->Bind(wxEVT_BUTTON, [this](wxCommandEvent &e) {
            wxGetApp().run_wizard(ConfigWizard::RR_USER, ConfigWizard::SP_PRINTERS);
            });

        wxBoxSizer* h_sizer_title = new wxBoxSizer(wxHORIZONTAL);
        h_sizer_title->Add(p->m_printer_icon, 0, wxALIGN_CENTRE | wxLEFT, FromDIP(SidebarProps::TitlebarMargin()));
        h_sizer_title->AddSpacer(FromDIP(SidebarProps::ElementSpacing()));
        h_sizer_title->Add(p->m_text_printer_settings, 0, wxALIGN_CENTER);
        h_sizer_title->AddStretchSpacer();
        h_sizer_title->Add(p->m_printerinfo_syncbtn, 0, wxALIGN_CENTER);
        h_sizer_title->wxSizer::AddSpacer(FromDIP(10));
        h_sizer_title->Add(p->m_printer_setting, 0, wxALIGN_CENTER);
        h_sizer_title->AddSpacer(FromDIP(SidebarProps::TitlebarMargin()));
        h_sizer_title->SetMinSize(-1, 3 * em);

        p->m_panel_printer_title->SetSizer(h_sizer_title);
        p->m_panel_printer_title->Layout();

        // add printer title
        scrolled_sizer->Add(p->m_panel_printer_title, 0, wxEXPAND | wxALL, 0);
        p->m_panel_printer_title->Bind(wxEVT_LEFT_UP, [this] (auto & e) {
            if (p->m_panel_printer_content->GetMaxHeight() == 0)
                p->m_panel_printer_content->SetMaxSize({-1, -1});
            else
                p->m_panel_printer_content->SetMaxSize({-1, 0});
            m_scrolled_sizer->Layout();
        });

        // add spliter 2
        auto spliter_2 = new ::StaticLine(p->scrolled);
        spliter_2->SetLineColour("#CECECE");
        scrolled_sizer->Add(spliter_2, 0, wxEXPAND);


        /*************************** 2. add printer content ************************/
        p->m_panel_printer_content = new wxPanel(p->scrolled, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        p->m_panel_printer_content->SetBackgroundColour(wxColour(255, 255, 255));

        StateColor panel_bd_col(std::pair<wxColour, int>(wxColour(0x00AE42), StateColor::Pressed),
                                std::pair<wxColour, int>(wxColour(0x00AE42), StateColor::Hovered),
                                std::pair<wxColour, int>(wxColour(0xEEEEEE), StateColor::Normal));

        p->panel_printer_preset = new StaticBox(p->m_panel_printer_content);
        p->panel_printer_preset->SetCornerRadius(8);
        p->panel_printer_preset->SetBorderColor(panel_bd_col);
        p->panel_printer_preset->SetMinSize(PRINTER_PANEL_SIZE_SMALL);
        p->panel_printer_preset->Bind(wxEVT_LEFT_DOWN, [this](auto& evt) { p->combo_printer->wxEvtHandler::ProcessEvent(evt); });

        PlaterPresetComboBox* combo_printer = new PlaterPresetComboBox(p->panel_printer_preset, Preset::TYPE_PRINTER);
        combo_printer->SetWindowStyle(combo_printer->GetWindowStyle() & ~wxALIGN_MASK | wxALIGN_LEFT);
        combo_printer->SetBorderWidth(0);
        
        ScalableBitmap bitmap_printer(p->panel_printer_preset, "printer_placeholder", 48);
        p->image_printer = new wxStaticBitmap(p->panel_printer_preset, wxID_ANY, bitmap_printer.bmp(), wxDefaultPosition,
                                                                 PRINTER_THUMBNAIL_SIZE, 0);
        p->image_printer->Bind(wxEVT_LEFT_DOWN, [this](auto& evt) { p->combo_printer->wxEvtHandler::ProcessEvent(evt); });
        
        p->combo_printer = combo_printer;

        // 绑定 combo 内部按钮的事件处理（按钮现在在 combo 内部）
        combo_printer->bind_edit_button_handler([this, combo_printer]() {
                p->editing_filament = -1;
                if (combo_printer->switch_to_tab())
                    p->editing_filament = 0;
            });
        
        combo_printer->bind_connection_button_handler([this]() {
                wxGetApp().sm_disconnect_current_machine();
                PhysicalPrinterDialog dlg(this->GetParent());
                dlg.ShowModal();
            });

        combo_printer->bind_machine_connecting_button_handler([this]() {
            // machine_connecting_btn 的处理逻辑（如果有的话）
        });
        
        // 显示编辑按钮（鼠标悬停时原来会显示，现在直接显示）
        combo_printer->set_show_edit_button(true);
        
        // 设置按钮tooltip
        combo_printer->set_connection_tooltip(_L("Connect to printer"));
        combo_printer->set_machine_connecting_tooltip(_L("The machine has been connected and is currently in working mode"));

        // 外部按钮已集成到 combo 内部，不再需要创建
        // 保留变量引用以避免编译错误，但设为 nullptr
        connection_btn = nullptr;
        machine_connecting_btn = nullptr;

        wxBoxSizer* vsizer_printer = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* hsizer_printer = new wxBoxSizer(wxHORIZONTAL);

        wxBoxSizer* vsizer = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* hsizer = new wxBoxSizer(wxHORIZONTAL);

        // 简化后的布局：打印机图片 + combo_printer（内含所有按钮）
        combo_printer->SetWindowStyle(combo_printer->GetWindowStyle() & ~wxALIGN_MASK | wxALIGN_LEFT);

        hsizer->Add(p->image_printer, 0, wxLEFT | wxALIGN_CENTER, FromDIP(4));
        hsizer->Add(combo_printer, 1, wxALIGN_CENTRE | wxLEFT | wxRIGHT, FromDIP(6));
        hsizer->AddSpacer(FromDIP(10));
        p->panel_printer_preset->SetSizer(hsizer);

        hsizer_printer->Add(p->panel_printer_preset, 1, wxEXPAND, 0);
        vsizer_printer->AddSpacer(FromDIP(4));
        vsizer_printer->Add(hsizer_printer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, FromDIP(4));
        vsizer_printer->AddSpacer(FromDIP(10));

        /*vsizer_printer->AddSpacer(FromDIP(16));
        hsizer_printer->Add(p->image_printer, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, FromDIP(3));
        hsizer_printer->Add(combo_printer, 1, wxALIGN_CENTER_VERTICAL | wxLEFT, FromDIP(3));
        hsizer_printer->Add(edit_btn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, FromDIP(3));
        hsizer_printer->Add(FromDIP(8), 0, 0, 0, 0);
        hsizer_printer->Add(connection_btn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, FromDIP(3));
        hsizer_printer->Add(machine_connecting_btn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, FromDIP(3));
        hsizer_printer->Add(FromDIP(8), 0, 0, 0, 0);

        vsizer_printer->Add(hsizer_printer, 0, wxEXPAND, 0);*/

        // Bed type selection
        // 创建一个像打印机选择那样的容器
        p->panel_printer_preset = new StaticBox(p->m_panel_printer_content, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                                wxTAB_TRAVERSAL | wxBORDER_NONE);
        p->panel_printer_preset->SetCornerRadius(8);
        StateColor panel_bd_col1(std::pair<wxColour, int>(wxColour(0x00AE42), StateColor::Pressed),
                            std::pair<wxColour, int>(wxColour(0x00AE42), StateColor::Hovered),
                            std::pair<wxColour, int>(wxColour(0xEEEEEE), StateColor::Normal));
        // p->panel_printer_preset->SetBorderColor(panel_bd_col1);
        // p->panel_printer_preset->SetMinSize(PRINTER_PANEL_SIZE_SMALL);

        // 创建Bed type选择控件
        wxBoxSizer* bed_type_sizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* bed_type_title = new wxStaticText(p->panel_printer_preset, wxID_ANY, _L("Bed type"));
        bed_type_title->Wrap(-1);
        bed_type_title->SetFont(Label::Body_14);
        m_bed_type_list = new ComboBox(p->panel_printer_preset, wxID_ANY, wxString(""), wxDefaultPosition, {-1, FromDIP(30)}, 0, nullptr, wxCB_READONLY);
        const ConfigOptionDef* bed_type_def = print_config_def.get("curr_bed_type");
        if (bed_type_def && bed_type_def->enum_keys_map) {
            for (const auto& item : bed_type_def->enum_labels)
                m_bed_type_list->AppendString(_L(item));
            for (const auto& v : bed_type_def->enum_values)
                m_bed_type_combo_enum_values.push_back(v);
        }

        // 添加链接事件等
        bed_type_title->Bind(wxEVT_ENTER_WINDOW, [bed_type_title, this](wxMouseEvent &e) {
            e.Skip();
            auto font = bed_type_title->GetFont();
            font.SetUnderlined(true);
            bed_type_title->SetFont(font);
            SetCursor(wxCURSOR_HAND);
        });
        bed_type_title->Bind(wxEVT_LEAVE_WINDOW, [bed_type_title, this](wxMouseEvent &e) {
            e.Skip();
            auto font = bed_type_title->GetFont();
            font.SetUnderlined(false);
            bed_type_title->SetFont(font);
            SetCursor(wxCURSOR_ARROW);
        });
        bed_type_title->Bind(wxEVT_LEFT_UP, [bed_type_title, this](wxMouseEvent &e) {
            wxLaunchDefaultBrowser("https://github.com/SoftFever/OrcaSlicer/wiki/bed-types");
        });

        AppConfig *app_config = wxGetApp().app_config;
        std::string str_bed_type = app_config->get("curr_bed_type");
        int bed_type_value = atoi(str_bed_type.c_str());
        // hotfix: btDefault is added as the first one in BedType, and app_config should not be btDefault
        if (bed_type_value == 0) {
            app_config->set("curr_bed_type", "1");
            bed_type_value = 1;
        }

        int bed_type_idx = bed_type_value - 1;
        m_bed_type_list->Select(bed_type_idx);

        // 布局Bed type控件
        bed_type_sizer->Add(bed_type_title, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, FromDIP(10));
        bed_type_sizer->Add(m_bed_type_list, 1, wxLEFT | wxRIGHT | wxEXPAND, FromDIP(0));
        p->panel_printer_preset->SetSizer(bed_type_sizer);

        // 添加到垂直布局
        vsizer_printer->Add(p->panel_printer_preset, 0, wxEXPAND | wxALL, FromDIP(4));
        vsizer_printer->AddSpacer(FromDIP(8));

        auto& project_config = wxGetApp().preset_bundle->project_config;
        BedType bed_type = (BedType)bed_type_value;
        project_config.set_key_value("curr_bed_type", new ConfigOptionEnum<BedType>(bed_type));

        p->m_panel_printer_content->SetSizer(vsizer_printer);
        p->m_panel_printer_content->Layout();
        scrolled_sizer->Add(p->m_panel_printer_content, 0, wxEXPAND, 0);

        // 创建Nozzle notebook的容器
        StaticBox* nozzle_container = new StaticBox(p->m_panel_printer_content, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                                    wxTAB_TRAVERSAL | wxBORDER_NONE);
        nozzle_container->SetCornerRadius(8);
        // nozzle_container->SetBorderColor(panel_bd_col);

        // 创建notebook
        p->m_nozzle_notebook = new CustomNotebook(nozzle_container, wxID_ANY);

        // 创建nozzle_sizer并添加notebook
        wxBoxSizer* nozzle_sizer = new wxBoxSizer(wxVERTICAL);
        nozzle_sizer->Add(p->m_nozzle_notebook, 1, wxEXPAND | wxALL, FromDIP(0));
        nozzle_container->SetSizer(nozzle_sizer);
        nozzle_container->SetMinSize(wxSize(-1, FromDIP(80)));

        // 添加到主布局
        vsizer_printer->Add(nozzle_container, 0, wxEXPAND | wxALL, FromDIP(4));

        // Initialize nozzle settings
        update_nozzle_settings();
    }

    {
    // add filament title
    p->m_panel_filament_title = new StaticBox(p->scrolled, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_NONE);
    p->m_panel_filament_title->SetBackgroundColor(title_bg);
    p->m_panel_filament_title->SetBackgroundColor2(0xF1F1F1);
    p->m_panel_filament_title->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent &e) {
        if (e.GetPosition().x > (p->m_flushing_volume_btn->IsShown()
                ? p->m_flushing_volume_btn->GetPosition().x : (p->m_bpButton_ams_filament->GetPosition().x - FromDIP(30))))
            return;
        if (p->m_panel_filament_content->GetMaxHeight() == 0)
            p->m_panel_filament_content->SetMaxSize({-1, -1});
        else
            p->m_panel_filament_content->SetMaxSize({-1, 0});
        m_scrolled_sizer->Layout();
    });

    wxBoxSizer* bSizer39;
    bSizer39 = new wxBoxSizer( wxHORIZONTAL );
    p->m_filament_config_icon = new ScalableButton(p->m_panel_filament_title, wxID_ANY, "filament_group");
    bSizer39->Add(p->m_filament_config_icon, 0, wxALIGN_CENTER | wxLEFT, FromDIP(SidebarProps::TitlebarMargin()));
    p->m_staticText_filament_settings = new Label(p->m_panel_filament_title, _L("Filament Management"), LB_PROPAGATE_MOUSE_EVENT);
    bSizer39->Add( p->m_staticText_filament_settings, 0, wxALIGN_CENTER | wxLEFT, FromDIP(SidebarProps::TitlebarMargin()));
    bSizer39->Add(FromDIP(10), 0, 0, 0, 0);
    bSizer39->SetMinSize(-1, FromDIP(30));

    p->m_panel_filament_title->SetSizer( bSizer39 );
    p->m_panel_filament_title->Layout();
    auto spliter_1 = new ::StaticLine(p->scrolled);
    spliter_1->SetLineColour("#A6A9AA");
    scrolled_sizer->Add(spliter_1, 0, wxEXPAND);
    scrolled_sizer->Add(p->m_panel_filament_title, 0, wxEXPAND | wxALL, 0);
    auto spliter_2 = new ::StaticLine(p->scrolled);
    spliter_2->SetLineColour("#CECECE");
    scrolled_sizer->Add(spliter_2, 0, wxEXPAND);

    bSizer39->AddStretchSpacer(1);

    // BBS
    // add wiping dialog
    //wiping_dialog_button->SetFont(wxGetApp().normal_font());
    p->m_flushing_volume_btn = new Button(p->m_panel_filament_title, _L("Flushing volumes"));
    p->m_flushing_volume_btn->SetStyle(ButtonStyle::Confirm, ButtonType::Compact);
    p->m_flushing_volume_btn->SetId(wxID_RESET);
    p->m_flushing_volume_btn->Bind(wxEVT_BUTTON, ([parent](wxCommandEvent &e)
        {
            auto& project_config = wxGetApp().preset_bundle->project_config;
            const std::vector<double>& init_matrix = (project_config.option<ConfigOptionFloats>("flush_volumes_matrix"))->values;
            const std::vector<double>& init_extruders = (project_config.option<ConfigOptionFloats>("flush_volumes_vector"))->values;
            ConfigOptionFloat* flush_multi_opt = project_config.option<ConfigOptionFloat>("flush_multiplier");
            float flush_multiplier = flush_multi_opt ? flush_multi_opt->getFloat() : 1.f;

            const std::vector<std::string> extruder_colours = wxGetApp().plater()->get_extruder_colors_from_plater_config(nullptr, false);
            const auto& full_config = wxGetApp().preset_bundle->full_config();
            const auto& extra_flush_volumes = get_min_flush_volumes(full_config);
            WipingDialog dlg(parent, cast<float>(init_matrix), cast<float>(init_extruders), extruder_colours, extra_flush_volumes, flush_multiplier);
            if (dlg.ShowModal() == wxID_OK) {
                std::vector<float> matrix = dlg.get_matrix();
                std::vector<float> extruders = dlg.get_extruders();
                (project_config.option<ConfigOptionFloats>("flush_volumes_matrix"))->values = std::vector<double>(matrix.begin(), matrix.end());
                (project_config.option<ConfigOptionFloats>("flush_volumes_vector"))->values = std::vector<double>(extruders.begin(), extruders.end());
                (project_config.option<ConfigOptionFloat>("flush_multiplier"))->set(new ConfigOptionFloat(dlg.get_flush_multiplier()));

                wxGetApp().preset_bundle->export_selections(*wxGetApp().app_config);

                wxGetApp().plater()->update_project_dirty_from_presets();
                wxPostEvent(parent, SimpleEvent(EVT_SCHEDULE_BACKGROUND_PROCESS, parent));
            }
        }));

    bSizer39->Add(p->m_flushing_volume_btn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, FromDIP(5));
    bSizer39->Hide(p->m_flushing_volume_btn);

    // Batch color-match mapping button
    p->m_btn_batch_match = new Button(p->m_panel_filament_title, _L("Color Mixing Match"));
    p->m_btn_batch_match->SetStyle(ButtonStyle::Confirm, ButtonType::Compact);
    p->m_btn_batch_match->SetToolTip(_L("Automatically calculate the color mixing scheme that best matches the original model colors and complete color mapping.\n"
                                        "Note:\n"
                                        "1.Color mixing match is based on the official recommended CMYG filaments. The matched colors may differ from the original model.\n"
                                        "2.The order of the Color Mapping list may differ from that of the Color Mixing list."));
    p->m_btn_batch_match->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        if (!wxGetApp().preset_bundle) return;
        // No loaded model → batch match has nothing to map. Surface as a confirmation
        // dialog (not an error — these are "please do X first" hints, not failures)
        // BEFORE opening the dialog, using the same RichMessageDialog style as the
        // filament-sync prompts (caption "Color Mixing Match", "Got it" button, screen-centred).
        if (p->plater->model().objects.empty()) {
            RichMessageDialog dlg(this,
                _L("No model detected. Import a multi-color model to continue."),
                _L("Color Mixing Match"), wxOK);
            dlg.SetOKLabel(_L("Got it"));
            dlg.CentreOnScreen();
            dlg.ShowModal();
            return;
        }
        ConfigOptionStrings* co = wxGetApp().preset_bundle->project_config.option<ConfigOptionStrings>("filament_colour");
        const std::vector<std::string> colors = co ? co->values : std::vector<std::string>{};
        if (colors.size() < 2) {
            RichMessageDialog dlg(this,
                _L("Color Mixing Match is unavailable when only one filament is added to Filament Management."),
                _L("Color Mixing Match"), wxOK);
            dlg.SetOKLabel(_L("Got it"));
            dlg.CentreOnScreen();
            dlg.ShowModal();
            return;
        }
        MixedFilamentBatchDialog dlg(this);
        if (dlg.ShowModal() != wxID_OK) return;
        const BatchMatchResult& result = dlg.GetResult();
        if (!result.success) return;

        // Applying batch-match results involves palette rewrite, mixed-filament
        // creation, painting remap and cleanup — all synchronous on the main
        // thread. Show a determinate progress dialog so the user sees real
        // progress instead of "not responding". Profiling showed ~96% of apply
        // time is the physical-filament deletion loop inside cleanup_unused_, so
        // the bar is mapped by time share: quick pre-steps → 0..5, the deletion
        // loop → 5..95 (reported back per-deletion via on_progress), the final UI
        // refresh → 95..100. wxWidgets pitfall (§70): Update() pumps the event
        // loop and its return value reflects skip/close state; there is no abort
        // button here so it's consumed but not acted on. wxPD_APP_MODAL blocks
        // interaction with the main window while the apply runs.
        ProgressDialog progress(_L("Loading"),
                                _L("Updating the Filaments and Color Mixing list..."),
                                100, find_toplevel_parent(this),
                                wxPD_APP_MODAL | wxPD_AUTO_HIDE);
        const wxString kMsg = _L("Updating the Filaments and Color Mixing list...");
        auto set_progress = [&](int pct) {
            // Return value intentionally not acted on (no wxPD_CAN_ABORT button),
            // but consumed to satisfy §70 ("check the return value of Update()").
            (void) progress.Update(pct, kMsg);
        };
        set_progress(1);

        auto& mgr = wxGetApp().preset_bundle->mixed_filaments;

        // Snapshot the dialog-time virtual-id -> stable_id mapping BEFORE any
        // palette or filament-count change.  The dialog captured each mapping's
        // source_extruder_ids ({slot index + 1}) against exactly this
        // enumeration; stable_ids survive the auto_generate rebuild that
        // set_num_filaments runs in recommended mode, so this table lets the
        // in-place / translate steps below re-find existing mixed rows by
        // IDENTITY.  (Display colors are refreshed to the NEW palette before
        // those steps run, so a color key would silently never match there.)
        std::unordered_map<unsigned int, uint64_t> dialog_vid_to_sid;
        const size_t dialog_num_physical = wxGetApp().preset_bundle->filament_presets.size();
        {
            unsigned int vid = static_cast<unsigned int>(dialog_num_physical) + 1;
            for (const MixedFilament& mf : mgr.mixed_filaments()) {
                if (!mf.enabled || mf.deleted) continue;
                dialog_vid_to_sid[vid++] = mf.stable_id;
            }
        }

        // NOTE: deliberately NO take_snapshot() here. The UndoRedo stack only
        // captures the Model (object painting), not the preset_bundle palette
        // or mixed_filaments. Snapshotting batch match would make Ctrl+Z revert
        // painting while leaving the expanded CMYG palette in place → silent
        // wrong colors. Re-enable only after UndoRedo covers preset_bundle.
        // For recommended mode, apply CMYG to the first 4 physical slots.
        // < 4 filaments: expand to 4.  >= 4 filaments: keep every slot THROUGH
        // this stage (virtual ids / add_batch compute over the full slot
        // space).  The batch match is a project-wide re-plan of the filament
        // system, so cleanup_unused_filaments_after_batch_match below removes
        // whichever physical slots the match did not select
        // (result.selected_physical_ids = {1..4} in recommended mode).
        std::vector<std::string> colors_vec;
        if (result.is_recommended_mode && result.recommended_physical_colors.size() >= 4) {
            const auto& cm = result.recommended_physical_colors;
            auto* pb = wxGetApp().preset_bundle;
            auto* fc = pb->project_config.option<ConfigOptionStrings>("filament_colour");

            const size_t current_count = pb->filament_presets.size();
            const size_t target_count  = std::max<size_t>(4, current_count);

            // Build full palette: CMYG in slots 1-4, original in 5+.
            colors_vec = fc ? fc->values : std::vector<std::string>{};
            colors_vec.resize(target_count);

            // Recommended-mode colors are plain single-color filaments.
            // Clear any left-over dual-color / gradient metadata on the
            // first 4 slots so the UI renders the new palette instead of
            // the old multi-color swatches.  (filament_multi_colors is
            // indexed the same as filament_colour; a non-empty entry
            // takes precedence over the single-color value.)
            auto* multi_colors = pb->project_config.option<ConfigOptionStrings>("filament_multi_colors");
            auto* color_modes  = pb->project_config.option<ConfigOptionInts>("filament_colour_mode");
            if (multi_colors)
                multi_colors->values.resize(target_count);
            if (color_modes)
                color_modes->values.resize(target_count);
            for (size_t i = 0; i < 4 && i < cm.size(); ++i) {
                colors_vec[i] = cm[i];
                if (multi_colors)
                    multi_colors->values[i].clear();
                if (color_modes)
                    color_modes->values[i] = 0;
            }

            // Write before set_num_filaments so auto_generate sees CMYG palette.
            if (fc) fc->values = colors_vec;

            // Snapshot the old mixed list BEFORE set_num_filaments clears
            // custom entries, so we can build a proper old→new remap that
            // covers the existing painted virtual IDs (review item R3).
            const std::vector<MixedFilament> old_mixed_snapshot = pb->mixed_filaments.mixed_filaments();

            pb->set_num_filaments(static_cast<unsigned int>(target_count), std::vector<std::string>{});

            // Restore custom entries that were wiped by clear_custom_entries
            // in update_multi_material_filament_presets.  add_batch_custom_
            // filaments below will manage the batch-matched rows; we just
            // need to keep the pre-existing custom rows alive so their
            // stable_ids survive into the in-place edit block above and the
            // translation step below.
            {
                const std::string saved = pb->project_config.opt_string("mixed_filament_definitions");
                if (!saved.empty())
                    pb->mixed_filaments.load_custom_entries(saved, colors_vec);
            }

            // Build a remap from old→new virtual IDs so on_filaments_change
            // correctly remaps existing painted mixed-IDs before
            // apply_batch_match_to_model runs.  A physical-count change shifts
            // every mixed row's virtual id even when the rows are structurally
            // identical (operator== ignores display_color), so run the remap
            // whenever the count changes, not only when the row list differs.
            if (current_count != target_count ||
                old_mixed_snapshot != pb->mixed_filaments.mixed_filaments()) {
                pb->update_mixed_filament_id_remap(
                    old_mixed_snapshot, current_count, target_count);
            }

            // Remap config-level extruder references (object/volume/layer) with
            // the remap just built: the triangle remap inside on_filaments_change
            // only touches mmu_segmentation_facets, so without this a config
            // entry on a mixed row would stay on its stale pre-expansion id
            // (e.g. vid 3 → 5 on a 2→4 physical-count expansion). Scoped to this
            // batch-match branch only — other paths that build a remap (row
            // delete/enable, manual add/remove) are intentionally left unchanged.
            // Copy, don't borrow: last_filament_id_remap() returns a reference
            // into PresetBundle's buffer, which on_filaments_change below
            // consumes (moves + clears). A copy keeps this block independent
            // of that consume ordering.
            const std::vector<unsigned int> batch_remap = pb->last_filament_id_remap();
            if (!batch_remap.empty()) {
                const size_t total_filaments = target_count + pb->mixed_filaments.enabled_count();
                // NONE is the default: a stale id NOT in the remap must not
                // stay identity, because after a 2-to-4 renumbering the old
                // mixed id 3 would alias onto physical slot 3 and silently
                // reassign the object.  The loop overlays the actual remap on
                // top of the NONE-filled baseline.
                EnforcerBlockerStateMap batch_state_map;
                batch_state_map.fill(EnforcerBlockerType::NONE);
                for (size_t i = 1; i < batch_state_map.size(); ++i) {
                    const unsigned int mapped = i < batch_remap.size() ? batch_remap[i] : 0;
                    if (mapped == 0 || mapped >= batch_state_map.size() || mapped > total_filaments)
                        continue;  // stays NONE: deleted/expired row, config falls back to default
                    batch_state_map[i] = EnforcerBlockerType(mapped);
                }
                auto remap_config_extruder = [&batch_state_map](ModelConfig& cfg) {
                    if (!cfg.has("extruder"))
                        return;
                    const int eid = cfg.extruder();
                    if (eid <= 0 || static_cast<size_t>(eid) >= batch_state_map.size())
                        return;
                    const unsigned int mapped = static_cast<unsigned int>(batch_state_map[static_cast<size_t>(eid)]);
                    if (mapped == 0) {
                        // Deleted/expired row: revert to default.  Set the key to
                        // 0 rather than erasing it — a missing "extruder" would
                        // make cfg.extruder() (opt_int, nullptr on absent key) a
                        // dangling dereference for any unprotected reader, and an
                        // explicit 0 matches the out-of-range normalization in
                        // update_filament_values_for_items (GUI_ObjectList.cpp).
                        // Objects/volumes with extruder 0 resolve to "default"
                        // via ModelVolume::extruder_id()'s inherit-from-object
                        // fallback, so inheriting children stay "default" too.
                        cfg.set("extruder", 0);
                    } else if (mapped != static_cast<unsigned int>(eid)) {
                        cfg.set_key_value("extruder", new ConfigOptionInt(static_cast<int>(mapped)));
                    }
                };
                for (ModelObject* mo : wxGetApp().model().objects) {
                    remap_config_extruder(mo->config);
                    for (ModelVolume* mv : mo->volumes)
                        remap_config_extruder(mv->config);
                    for (auto& lr : mo->layer_config_ranges)
                        remap_config_extruder(lr.second);
                }
            }

            // Write Full Spectrum to slots 1-4 only when the preset is
            // selectable under the current printer (present + visible +
            // compatible, matching the filament combobox filter).
            const std::string full_spectrum_preset = full_spectrum_preset_name();
            const Preset*     fs_preset = pb->filaments.find_preset(full_spectrum_preset);
            if (fs_preset != nullptr && fs_preset->is_visible && fs_preset->is_compatible) {
                for (size_t i = 0; i < std::min<size_t>(4, target_count); ++i)
                    pb->set_filament_preset(i, full_spectrum_preset);
            }

            wxGetApp().plater()->on_filaments_change(static_cast<int>(target_count));
        } else {
            ConfigOptionStrings* co = wxGetApp().preset_bundle->project_config.option<ConfigOptionStrings>("filament_colour");
            colors_vec = co ? co->values : std::vector<std::string>{};
        }

        BatchMatchResult model_result = result; // mutable copy — used for in-place edits + apply/cleanup

        // In-place recipe update for EXISTING custom mixed filaments: overwrite
        // the row's recipe with the new-palette match (mapping.recipe is already
        // the best new-palette approximation of that model color), keeping
        // stable_id / virtual id / painting.  This avoids creating a duplicate
        // new mixed for colors the user already has and — because the virtual id
        // and painting are untouched — sidesteps the cascade/erase that would
        // otherwise lose painting when a component physical is dropped.  Rows
        // are matched by IDENTITY (dialog-time source id -> stable_id -> current
        // row), NOT by display color: recommended mode refreshes display colors
        // before this block runs.  Auto rows never match (their vids resolve to
        // rows with custom == false; auto_generate owns them).
        {
            const size_t cur_num_physical = colors_vec.size();
            bool any_in_place = false;
            for (auto& mapping : model_result.mappings) {
                if (mapping.is_pure_recipe || mapping.in_place_edited) continue;
                const MixedColorMatchRecipeResult& r = mapping.recipe;
                // Only write recipes add_batch would accept: valid, in-range,
                // distinct components.  Anything else falls through to the
                // add_batch path, which clamps and validates on its own.
                if (!r.valid) continue;
                if (r.component_a < 1 || r.component_a > cur_num_physical
                    || r.component_b < 1 || r.component_b > cur_num_physical
                    || r.component_a == r.component_b) continue;
                // Resolve the dialog-time source id to an existing row identity.
                uint64_t sid = 0;
                for (unsigned int src : mapping.source_extruder_ids) {
                    auto it = dialog_vid_to_sid.find(src);
                    if (it != dialog_vid_to_sid.end() && it->second != 0) {
                        sid = it->second;
                        break;
                    }
                }
                if (sid == 0) continue; // no existing mixed row behind this color
                // Locate that row in the CURRENT mixed list; record its vid.
                MixedFilament* target     = nullptr;
                unsigned int   target_vid = 0;
                unsigned int   vid        = static_cast<unsigned int>(cur_num_physical) + 1;
                for (MixedFilament& mf : mgr.mixed_filaments()) {
                    if (!mf.enabled || mf.deleted) continue;
                    if (mf.stable_id == sid) { target = &mf; target_vid = vid; break; }
                    ++vid;
                }
                // Only custom rows can be rewritten in place.  Auto rows are
                // owned by auto_generate; in this project auto mixed filaments
                // are off by default and not painted, so they never reach here.
                // If auto painting is ever enabled, revisit the R3 invariant
                // documented in cleanup_unused_filaments_after_batch_match.
                if (target == nullptr || !target->custom) continue;
                target->component_a                = r.component_a;
                target->component_b                = r.component_b;
                target->mix_b_percent              = r.mix_b_percent;
                target->gradient_component_ids     = r.gradient_component_ids;
                target->gradient_component_weights = r.gradient_component_weights;
                target->distribution_mode          = r.gradient_component_ids.empty()
                    ? int(MixedFilament::Simple) : int(MixedFilament::LayerCycle);
                target->gradient_enabled           = !r.gradient_component_ids.empty();
                target->manual_pattern.clear();   // MATCH recipes carry no manual pattern
                target->ratio_a = 1;              // MATCH recipes are 1:1 (equal-ratio blend)
                target->ratio_b = 1;
                target->ui_mode = 2;              // MATCH
                // Keep the mapping in the list, flagged: batch_entries and the
                // assigned-id fixup skip it, apply no-ops on it (source ==
                // target after translation below), and extract_batch_kept_sets
                // keeps the row via target_filament_id — so the edited row is
                // not deleted by the cleanup, regardless of whether it is painted.
                mapping.in_place_edited    = true;
                mapping.target_filament_id = target_vid;
                any_in_place = true;
            }
            // Refresh swatches so they reflect the new recipes even when the
            // batch below ends up empty (add_batch skips refresh in that case).
            if (any_in_place)
                mgr.refresh_display_colors(colors_vec);
        }

        // Translate mixed-slot source ids from the dialog epoch to the CURRENT
        // epoch.
        {
            const unsigned int cur_num_physical = static_cast<unsigned int>(colors_vec.size());
            for (auto& mapping : model_result.mappings) {
                std::vector<unsigned int> translated;
                translated.reserve(mapping.source_extruder_ids.size());
                for (unsigned int src : mapping.source_extruder_ids) {
                    if (src <= static_cast<unsigned int>(dialog_num_physical)) {
                        translated.push_back(src); // physical slot: identity
                        continue;
                    }
                    auto it = dialog_vid_to_sid.find(src);
                    if (it == dialog_vid_to_sid.end() || it->second == 0) continue;
                    unsigned int cur_vid = 0;
                    unsigned int vid     = cur_num_physical + 1;
                    for (const MixedFilament& mf : mgr.mixed_filaments()) {
                        if (!mf.enabled || mf.deleted) continue;
                        if (mf.stable_id == it->second) { cur_vid = vid; break; }
                        ++vid;
                    }
                    if (cur_vid != 0) translated.push_back(cur_vid);
                }
                mapping.source_extruder_ids = std::move(translated);
            }
        }

        std::vector<MixedFilamentBatchEntry> batch_entries;
        batch_entries.reserve(model_result.mappings.size());
        for (const auto& mapping : model_result.mappings) {
            // Skip condition MUST stay identical to the assigned-id fixup below —
            // `k` aligns mappings to assigned_ids by construction order.
            if (mapping.is_pure_recipe || mapping.in_place_edited) continue;
            MixedFilamentBatchEntry entry;
            entry.component_a     = mapping.recipe.component_a;
            entry.component_b     = mapping.recipe.component_b;
            entry.mix_b_percent   = mapping.recipe.mix_b_percent;
            entry.manual_pattern  = mapping.recipe.manual_pattern;
            entry.gradient_component_ids     = mapping.recipe.gradient_component_ids;
            entry.gradient_component_weights = mapping.recipe.gradient_component_weights;
            entry.distribution_mode = mapping.recipe.gradient_component_ids.empty()
                ? int(MixedFilament::Simple) : int(MixedFilament::LayerCycle);
            entry.display_color = mapping.matched_color.GetAsString(wxC2S_HTML_SYNTAX).ToStdString();
            batch_entries.push_back(std::move(entry));
        }
        std::vector<unsigned int> assigned_ids;
        set_progress(3);
        mgr.add_batch_custom_filaments(batch_entries, colors_vec, &assigned_ids);

        // Replace dialog-computed target ids with the actual virtual ids assigned by
        // add_batch_custom_filaments.
        {
            size_t k = 0;
            size_t dropped = 0;
            for (auto& mapping : model_result.mappings) {
                // MUST mirror the batch_entries skip condition above (k-alignment).
                if (mapping.is_pure_recipe || mapping.in_place_edited) continue;
                const unsigned int vid = (k < assigned_ids.size()) ? assigned_ids[k] : 0u;
                ++k;
                if (vid != 0u) {
                    mapping.target_filament_id = vid;
                } else {
                    mapping.target_filament_id = 0;
                    mapping.source_extruder_ids.clear();
                    ++dropped;
                }
            }
            if (dropped > 0)
                BOOST_LOG_TRIVIAL(warning)
                    << "Batch match: " << dropped << " recipe(s) dropped (cap "
                    << "or invalid components); regions left on original filament.";
        }
        // Apply matched recipes to model painting data
        set_progress(5);
        apply_batch_match_to_model(model_result);

        // Remove physical/mixed filaments left unreferenced by the match. This is
        // ~96% of apply time (per-deletion combo rebuild). Map the deletion loop's
        // (current, total) onto 5..95 so the bar advances steadily and reflects
        // real remaining work, not just a spinner.
        cleanup_unused_filaments_after_batch_match(
            model_result,
            [&set_progress](int current, int total) {
                // Map current/total ∈ [1,total] to 5..95. total==0 can't happen
                // here (the loop only runs when redundant_physical is non-empty),
                // but guard anyway to avoid divide-by-zero if the contract changes.
                const int span = (total > 0) ? (90 * current / total) : 0;
                set_progress(5 + span);
            });

        // cleanup already serializes; only panel refresh needed.
        set_progress(95);
        update_mixed_filament_panel(false);
        update_ui_from_settings();
        update_dynamic_filament_list();
        // Refresh the object list filament column so every row shows the
        // post-remap extruder ID.  Without this the list still displays the
        // old physical-slot IDs because the earlier on_filaments_change
        // refresh ran BEFORE apply_batch_match_to_model.
        obj_list()->update_objects_list_filament_column(colors_vec.size());
        // Force-refresh combo swatches in case filament count stayed the same
        // (Sidebar::on_filaments_change early-returns in that case).
        std::vector<PlaterPresetComboBox*>& fcombos = combos_filament();
        for (size_t i = 0; i < fcombos.size(); ++i) {
            if (fcombos[i]) fcombos[i]->update();
        }
        // §70: wxPD_AUTO_HIDE only fires at 100%, so reach 100 here for a clean
        // dismiss (otherwise the bar visibly aborts when the dialog leaves scope).
        set_progress(100);
    });
    bSizer39->Add(p->m_btn_batch_match, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, FromDIP(5));

    ams_btn = new ScalableButton(p->m_panel_filament_title, wxID_ANY, "ams_fila_sync", wxEmptyString, wxDefaultSize, wxDefaultPosition,
                                                 wxBU_EXACTFIT | wxNO_BORDER, false, 16); // ORCA match icon size with other icons as 16x16
    ams_btn->SetToolTip(_L("Synchronize filament list from AMS"));
    ams_btn->Bind(wxEVT_BUTTON, [this, scrolled_sizer](wxCommandEvent &e) {
        sync_ams_list();
    });
    p->m_bpButton_ams_filament = ams_btn;

    bSizer39->Add(ams_btn, 0, wxALIGN_CENTER | wxLEFT, FromDIP(SidebarProps::IconSpacing()));
    //bSizer39->Add(FromDIP(10), 0, 0, 0, 0 );

    ScalableButton* set_btn = new ScalableButton(p->m_panel_filament_title, wxID_ANY, "settings");
    set_btn->SetToolTip(_L("Set filaments to use"));
    set_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent &e) {
        p->editing_filament = -1;
        // wxGetApp().params_dialog()->Popup();
        // wxGetApp().get_tab(Preset::TYPE_FILAMENT)->restore_last_select_item();
        wxGetApp().run_wizard(ConfigWizard::RR_USER, ConfigWizard::SP_FILAMENTS);
        });
    p->m_bpButton_set_filament = set_btn;

    bSizer39->Add(set_btn, 0, wxALIGN_CENTER | wxLEFT, FromDIP(SidebarProps::IconSpacing()));
    bSizer39->AddSpacer(FromDIP(SidebarProps::TitlebarMargin()));

    // add filament content
    p->m_panel_filament_content = new wxPanel( p->scrolled, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    p->m_panel_filament_content->SetBackgroundColour( wxColour( 255, 255, 255 ) );

    //wxBoxSizer* bSizer_filament_content;
    //bSizer_filament_content = new wxBoxSizer( wxHORIZONTAL );

    // BBS:  filament double columns, wrapped in scrolled window (max 3 rows)
    p->sizer_filaments = new wxBoxSizer(wxHORIZONTAL);
    p->sizer_filaments->Add(new wxBoxSizer(wxVERTICAL), 1, wxEXPAND);
    p->sizer_filaments->Add(new wxBoxSizer(wxVERTICAL), 1, wxEXPAND);

    p->m_scrolled_filaments = new wxScrolledWindow(p->m_panel_filament_content, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
    p->m_scrolled_filaments->SetScrollRate(0, 5);
    p->m_scrolled_filaments->SetBackgroundColour(*wxWHITE);
    p->m_panel_scrolled_filament_content = new wxPanel(p->m_scrolled_filaments, wxID_ANY);
    p->m_panel_scrolled_filament_content->SetBackgroundColour(*wxWHITE);
    p->m_panel_scrolled_filament_content->SetSizer(p->sizer_filaments);
    auto* scrolled_fila_sizer = new wxBoxSizer(wxVERTICAL);
    scrolled_fila_sizer->Add(p->m_panel_scrolled_filament_content, 0, wxEXPAND);
    p->m_scrolled_filaments->SetSizer(scrolled_fila_sizer);

    p->combos_filament.push_back(nullptr);

    /* first filament item */
    // init_filament_combo(&p->combos_filament[0], 0);

    p->combos_filament[0] = new PlaterPresetComboBox(p->m_panel_scrolled_filament_content, Preset::TYPE_FILAMENT);
    auto combo_and_btn_sizer = new wxBoxSizer(wxHORIZONTAL);
    // BBS:  filament double columns
    combo_and_btn_sizer->AddSpacer(FromDIP(SidebarProps::ContentMargin()));
    if (p->combos_filament[0]->clr_picker) {
        p->combos_filament[0]->clr_picker->SetLabel("1");
        combo_and_btn_sizer->Add(p->combos_filament[0]->clr_picker, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT,FromDIP(SidebarProps::ElementSpacing()) - FromDIP(2)); // ElementSpacing - 2 (from combo box))
    }
    combo_and_btn_sizer->Add(p->combos_filament[0], 1, wxALL | wxEXPAND, FromDIP(2))->SetMinSize({-1, FromDIP(30) });

    /*ScalableButton* edit_btn = new ScalableButton(p->m_panel_filament_content, wxID_ANY, "edit");
    edit_btn->SetBackgroundColour(wxColour(255, 255, 255));
    edit_btn->SetToolTip(_L("Click to edit preset"));*/

    ScalableButton* edit_btn = new ScalableButton(p->m_panel_scrolled_filament_content, wxID_ANY, "menu_filament");
    edit_btn->SetToolTip(_L("Click to edit preset"));

    PlaterPresetComboBox* combobox = p->combos_filament[0];
    edit_btn->Bind(wxEVT_BUTTON, [this, edit_btn](wxCommandEvent) {
        auto    menu = p->plater->filament_action_menu(0);
        wxPoint pt{0, edit_btn->GetSize().GetHeight() + 10};
        pt                    = edit_btn->ClientToScreen(pt);
        pt                    = wxGetApp().mainframe->ScreenToClient(pt);
        p->m_menu_filament_id = 0;
        p->plater->PopupMenu(menu, (int) pt.x, pt.y);
    });
    combobox->edit_btn = edit_btn;

    combo_and_btn_sizer->Add(edit_btn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, FromDIP(SidebarProps::ElementSpacing()) - FromDIP(2)); // ElementSpacing - 2 (from combo box))
    combo_and_btn_sizer->AddSpacer(FromDIP(SidebarProps::ContentMargin()));

    p->combos_filament[0]->set_filament_idx(0);
    p->sizer_filaments->GetItem((size_t)0)->GetSizer()->Add(combo_and_btn_sizer, 1, wxEXPAND);

    //bSizer_filament_content->Add(p->sizer_filaments, 1, wxALIGN_CENTER | wxALL);
    wxSizer *sizer_filaments2 = new wxBoxSizer(wxVERTICAL);
    // --- Filaments title bar (same level as Color Mix title) ---
    p->m_panel_physical_filaments_title = new StaticBox(p->m_panel_filament_content, wxID_ANY, wxDefaultPosition,
                                                        wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_NONE);
    p->m_panel_physical_filaments_title->SetBackgroundColor(title_bg);
    p->m_panel_physical_filaments_title->SetBackgroundColor2(0xF1F1F1);
    p->m_panel_physical_filaments_title->SetMinSize(wxSize(-1, FromDIP(30)));
    p->m_panel_physical_filaments_title->SetMaxSize(wxSize(-1, FromDIP(30)));
    p->m_filament_icon = new ScalableButton(p->m_panel_physical_filaments_title, wxID_ANY, "filament");
    auto* physical_label = new Label(p->m_panel_physical_filaments_title, _L("Filaments"), LB_PROPAGATE_MOUSE_EVENT);
    auto* h_physical_title = new wxBoxSizer(wxHORIZONTAL);
    auto* white_left_f = new wxPanel(p->m_panel_physical_filaments_title, wxID_ANY, wxDefaultPosition, wxSize(FromDIP(SidebarProps::ContentMargin()), -1));
    white_left_f->SetBackgroundColour(*wxWHITE);
    h_physical_title->Add(white_left_f, 0, wxEXPAND | wxTOP | wxBOTTOM, 0);
    h_physical_title->Add(p->m_filament_icon, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, FromDIP(SidebarProps::TitlebarMargin()));
    h_physical_title->AddSpacer(FromDIP(SidebarProps::ElementSpacing()));
    h_physical_title->Add(physical_label, 0, wxALIGN_CENTER_VERTICAL);
    h_physical_title->AddStretchSpacer();

    // Sync filament button
    ScalableButton* sync_filament_btn = new ScalableButton(p->m_panel_physical_filaments_title, wxID_ANY, "sync_filament");
    sync_filament_btn->SetToolTip(_L("Sync Filament Information"));
    sync_filament_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& e) {
        show_sync_filament_dialog();
    });
    p->m_bpButton_sync_filament = sync_filament_btn;

    // Delete filament button — delegates to delete_filament for consistent remap behavior
    ScalableButton* del_btn = new ScalableButton(p->m_panel_physical_filaments_title, wxID_ANY, "delete_filament");
    del_btn->SetToolTip(_L("Remove last filament"));
    del_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent &e) {
        if (p->combos_filament.size() <= 1)
            return;
        delete_filament(size_t(-1), -1);
    });
    p->m_bpButton_del_filament = del_btn;

    // Add filament button
    ScalableButton* add_btn = new ScalableButton(p->m_panel_physical_filaments_title, wxID_ANY, "add_filament");
    add_btn->SetToolTip(_L("Add one filament"));
    add_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& e){
        if (p->combos_filament.size() >= MAXIMUM_EXTRUDER_NUMBER)
            return;
        PresetBundle* pb = wxGetApp().preset_bundle;
        if (!pb || pb->mixed_filaments.total_filaments(p->combos_filament.size()) >= MAXIMUM_FILAMENT_NUMBER)
            return;
        int filament_count = p->combos_filament.size() + 1;
        wxGetApp().plater()->confirm_auto_generated_gradients(filament_count);
        wxColour new_col = Plater::get_next_color_for_filament();
        std::string new_color = new_col.GetAsString(wxC2S_HTML_SYNTAX).ToStdString();
        pb->set_num_filaments(filament_count, new_color);
        wxGetApp().plater()->on_filaments_change(filament_count);
        wxGetApp().get_tab(Preset::TYPE_PRINT)->update();
        pb->export_selections(*wxGetApp().app_config);
        auto_calc_flushing_volumes(filament_count - 1);
    });
    p->m_bpButton_add_filament = add_btn;

    h_physical_title->Add(sync_filament_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(10));
    h_physical_title->Add(del_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(4));
    h_physical_title->Add(add_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(8));
    auto* white_right_f = new wxPanel(p->m_panel_physical_filaments_title, wxID_ANY, wxDefaultPosition, wxSize(FromDIP(SidebarProps::ContentMargin()), -1));
    white_right_f->SetBackgroundColour(*wxWHITE);
    h_physical_title->Add(white_right_f, 0, wxEXPAND | wxTOP | wxBOTTOM, 0);
    p->m_panel_physical_filaments_title->SetSizer(h_physical_title);
    p->m_panel_physical_filaments_title->Layout();

    if (p->combos_filament.size() <= 1)
        h_physical_title->Hide(p->m_bpButton_del_filament);

    sizer_filaments2->AddSpacer(FromDIP(8));
    sizer_filaments2->Add(p->m_panel_physical_filaments_title, 0, wxEXPAND, 0);
    sizer_filaments2->AddSpacer(FromDIP(8));
    sizer_filaments2->Add(p->m_scrolled_filaments, 0, wxEXPAND, 0);
    sizer_filaments2->AddSpacer(FromDIP(8));
    // --- Color Mix Panel (inside filament content, same level as filaments) ---
    init_color_mix_panel(p->m_panel_filament_content, sizer_filaments2);
    p->m_panel_filament_content->SetSizer(sizer_filaments2);
    p->m_panel_filament_content->Layout();
    scrolled_sizer->Add(p->m_panel_filament_content, 0, wxEXPAND, 0);
    }

    // --- Mixed Filaments Panel (Collapsible) ---
    {
    // Create title bar (StaticBox for collapsible header)
    p->m_panel_mixed_filaments_title = new StaticBox(p->scrolled, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_NONE);
    p->m_panel_mixed_filaments_title->SetBackgroundColor(title_bg);
    p->m_panel_mixed_filaments_title->SetBackgroundColor2(0xF1F1F1);

    // Create icon
    p->m_mixed_filaments_icon = new ScalableButton(p->m_panel_mixed_filaments_title, wxID_ANY, "filament");

    // Create title text
    p->m_staticText_mixed_filaments = new Label(p->m_panel_mixed_filaments_title, _L("Mixed Filaments"), LB_PROPAGATE_MOUSE_EVENT);

    // Create "Add Gradient" button
    p->m_btn_add_gradient = new Button(p->m_panel_mixed_filaments_title, _L("Add Gradient"));
    p->m_btn_add_gradient->SetStyle(ButtonStyle::Confirm, ButtonType::Compact);
    p->m_btn_add_gradient->Bind(wxEVT_BUTTON, [this](wxCommandEvent& e) {
        // Add gradient mixed filament
        if (wxGetApp().preset_bundle) {
            auto &mgr = wxGetApp().preset_bundle->mixed_filaments;
            // Get physical filament colors
            ConfigOptionStrings *co = wxGetApp().preset_bundle->project_config.option<ConfigOptionStrings>("filament_colour");
            std::vector<std::string> colors = co ? co->values : std::vector<std::string>();
            // Add a custom gradient (50% mix)
            mgr.add_custom_filament(1, 2, 50, colors);
            // Persist the custom entries so they survive the clear/load cycle in update_mixed_filament_panel
            if (ConfigOptionString *opt = wxGetApp().preset_bundle->project_config.option<ConfigOptionString>("mixed_filament_definitions"))
                opt->value = mgr.serialize_custom_entries();
            update_mixed_filament_panel(false);
            m_scrolled_sizer->Layout();
        }
    });

    // Create "Add Pattern" button
    p->m_btn_add_pattern = new Button(p->m_panel_mixed_filaments_title, _L("Add Pattern"));
    p->m_btn_add_pattern->SetStyle(ButtonStyle::Confirm, ButtonType::Compact);
    p->m_btn_add_pattern->Bind(wxEVT_BUTTON, [this](wxCommandEvent& e) {
        // Add pattern mixed filament
        if (wxGetApp().preset_bundle) {
            auto &mgr = wxGetApp().preset_bundle->mixed_filaments;
            // Get physical filament colors
            ConfigOptionStrings *co = wxGetApp().preset_bundle->project_config.option<ConfigOptionStrings>("filament_colour");
            std::vector<std::string> colors = co ? co->values : std::vector<std::string>();
            // Add a custom pattern filament (will be configured by user)
            mgr.add_custom_filament(1, 2, 50, colors);
            // Set manual pattern for the newly added filament
            auto &mfs = mgr.mixed_filaments();
            if (!mfs.empty()) {
                mfs.back().manual_pattern = "12";
                mfs.back().custom = true;
            }
            // Persist the custom entries so they survive the clear/load cycle in update_mixed_filament_panel
            if (ConfigOptionString *opt = wxGetApp().preset_bundle->project_config.option<ConfigOptionString>("mixed_filament_definitions"))
                opt->value = mgr.serialize_custom_entries();
            update_mixed_filament_panel(false);
            m_scrolled_sizer->Layout();
        }
    });

    // Create "Add Color" button
    p->m_btn_add_color = new Button(p->m_panel_mixed_filaments_title, _L("Add Color"));
    p->m_btn_add_color->SetStyle(ButtonStyle::Confirm, ButtonType::Compact);
    p->m_btn_add_color->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        if (wxGetApp().preset_bundle == nullptr)
            return;

        ConfigOptionStrings *co = wxGetApp().preset_bundle->project_config.option<ConfigOptionStrings>("filament_colour");
        const std::vector<std::string> colors = co ? co->values : std::vector<std::string>();
        if (colors.size() < 2)
            return;

        const MixedColorMatchRecipeResult recipe =
            prompt_best_color_match_recipe(this, colors, Plater::get_next_color_for_filament());
        if (recipe.cancelled)
            return;
        if (!recipe.valid) {
            show_error(this, _L("Unable to create a color match from the current physical filament colors."));
            return;
        }

        const MixedFilamentDisplayContext display_context = build_mixed_filament_display_context(colors);
        auto &mgr = wxGetApp().preset_bundle->mixed_filaments;
        mgr.set_display_context(display_context);
        mgr.add_custom_filament(recipe.component_a, recipe.component_b, recipe.mix_b_percent, colors);
        auto &mfs = mgr.mixed_filaments();
        if (!mfs.empty()) {
            MixedFilament &created = mfs.back();
            created.manual_pattern = recipe.manual_pattern;
            created.mix_b_percent  = recipe.mix_b_percent;
            created.gradient_component_ids = recipe.gradient_component_ids;
            created.gradient_component_weights = recipe.gradient_component_weights;
            created.pointillism_all_filaments = false;
            created.distribution_mode = recipe.gradient_component_ids.empty() ? int(MixedFilament::Simple) : int(MixedFilament::LayerCycle);
            created.custom = true;
            created.display_color = compute_color_match_recipe_display_color(recipe, display_context).GetAsString(wxC2S_HTML_SYNTAX).ToStdString();
        }

        if (ConfigOptionString *opt = wxGetApp().preset_bundle->project_config.option<ConfigOptionString>("mixed_filament_definitions"))
            opt->value = mgr.serialize_custom_entries();
        update_mixed_filament_panel(false);
        m_scrolled_sizer->Layout();
    });

    // Create "Add Color" button
    wxBoxSizer* h_sizer_mixed_title = new wxBoxSizer(wxHORIZONTAL);
    h_sizer_mixed_title->Add(p->m_mixed_filaments_icon, 0, wxALIGN_CENTER | wxLEFT, FromDIP(SidebarProps::TitlebarMargin()));
    h_sizer_mixed_title->AddSpacer(FromDIP(SidebarProps::ElementSpacing()));
    h_sizer_mixed_title->Add(p->m_staticText_mixed_filaments, 0, wxALIGN_CENTER);
    h_sizer_mixed_title->AddStretchSpacer();
    h_sizer_mixed_title->Add(p->m_btn_add_gradient, 0, wxALIGN_CENTER | wxRIGHT, FromDIP(SidebarProps::ElementSpacing()));
    h_sizer_mixed_title->Add(p->m_btn_add_pattern, 0, wxALIGN_CENTER | wxRIGHT, FromDIP(SidebarProps::ElementSpacing()));
    h_sizer_mixed_title->Add(p->m_btn_add_color, 0, wxALIGN_CENTER | wxRIGHT, FromDIP(SidebarProps::TitlebarMargin()));
    h_sizer_mixed_title->SetMinSize(-1, FromDIP(30));

    p->m_panel_mixed_filaments_title->SetSizer(h_sizer_mixed_title);
    p->m_panel_mixed_filaments_title->Layout();

    // Add splitter line before title
    auto spliter_mixed_1 = new ::StaticLine(p->scrolled);
    spliter_mixed_1->SetLineColour("#A6A9AA");
    scrolled_sizer->Add(spliter_mixed_1, 0, wxEXPAND);

    // Add title bar to scrolled sizer
    scrolled_sizer->Add(p->m_panel_mixed_filaments_title, 0, wxEXPAND | wxALL, 0);

    // Add splitter line after title
    auto spliter_mixed_2 = new ::StaticLine(p->scrolled);
    spliter_mixed_2->SetLineColour("#CECECE");
    scrolled_sizer->Add(spliter_mixed_2, 0, wxEXPAND);

    // Create content panel (collapsible)
    p->m_panel_mixed_filaments_content = new wxPanel(p->scrolled, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    {
        const bool is_dark = wxGetApp().dark_mode();
        p->m_panel_mixed_filaments_content->SetBackgroundColour(is_dark ? wxColour(45, 45, 49) : wxColour(255, 255, 255));
    }

    // Content sizer - store in member variable for later use
    p->m_sizer_mixed_filaments_content = new wxBoxSizer(wxVERTICAL);
    p->m_sizer_mixed_filaments_content->AddSpacer(FromDIP(SidebarProps::ContentMargin()));
    p->m_panel_mixed_filaments_content->SetSizer(p->m_sizer_mixed_filaments_content);
    p->m_panel_mixed_filaments_content->Layout();

    // Add content panel to scrolled sizer
    scrolled_sizer->Add(p->m_panel_mixed_filaments_content, 0, wxEXPAND, 0);

    // Bind collapse/expand event to title bar
    p->m_panel_mixed_filaments_title->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& e) {
        // Exclude button areas from collapse/expand
        int button_left = p->m_panel_mixed_filaments_title->GetClientSize().x;
        auto consider_button = [&button_left](wxWindow *button) {
            if (button && button->IsShown())
                button_left = std::min(button_left, button->GetPosition().x);
        };
        consider_button(p->m_btn_add_gradient);
        consider_button(p->m_btn_add_pattern);
        consider_button(p->m_btn_add_color);
        if (e.GetPosition().x > button_left - FromDIP(12))
            return;
        
        if (p->m_panel_mixed_filaments_content->GetMaxHeight() == 0)
            p->m_panel_mixed_filaments_content->SetMaxSize({-1, -1});
        else
            p->m_panel_mixed_filaments_content->SetMaxSize({-1, 0});
        m_scrolled_sizer->Layout();
    });

    // Initially hidden until 2+ filaments
    p->m_panel_mixed_filaments_title->Hide();
    p->m_panel_mixed_filaments_content->Hide();
    }

    {
    //add project title
    auto params_panel = ((MainFrame*)parent->GetParent())->m_param_panel;
    if (params_panel) {
        params_panel->get_top_panel()->Reparent(p->scrolled);
        auto spliter_1 = new ::StaticLine(p->scrolled);
        spliter_1->SetLineColour("#A6A9AA");
        scrolled_sizer->Add(spliter_1, 0, wxEXPAND);
        scrolled_sizer->Add(params_panel->get_top_panel(), 0, wxEXPAND);
        auto spliter_2 = new ::StaticLine(p->scrolled);
        spliter_2->SetLineColour("#CECECE");
        scrolled_sizer->Add(spliter_2, 0, wxEXPAND);
    }

    //add project content
    p->sizer_params = new wxBoxSizer(wxVERTICAL);

    // ORCA: Update search box to modern style
    p->m_search_bar = new StaticBox(p->scrolled);
    p->m_search_bar->SetCornerRadius(0);
    p->m_search_bar->SetBorderColor(wxColour("#CECECE"));

    p->m_search_item = new TextInput(p->m_search_bar, wxEmptyString, wxEmptyString, "", wxDefaultPosition, wxDefaultSize, 0 | wxBORDER_NONE);
    p->m_search_item->SetIcon(*BitmapCache().load_svg("search", FromDIP(16), FromDIP(16))); // ORCA: Add search icon to search box

    wxTextCtrl* text_ctrl = p->m_search_item->GetTextCtrl();
    text_ctrl->SetHint(_L("Search plate, object and part."));
    text_ctrl->SetForegroundColour(wxColour("#262E30"));
    text_ctrl->SetFont(Label::Body_13);
    text_ctrl->SetSize(wxSize(-1, FromDIP(16))); // Centers text vertically

    text_ctrl->Bind(wxEVT_SET_FOCUS, [this](wxFocusEvent& e) {
        if (p->dia->IsShown()) {
            e.Skip();
            return;
        }
        p->m_search_bar->SetBorderColor(wxColour("#009688"));
        wxPoint pos = this->p->m_search_bar->ClientToScreen(wxPoint(0, 0));
#ifndef __WXGTK__
        pos.y += this->p->m_search_bar->GetRect().height;
#else
        this->p->m_search_item->Enable(false);
#endif
        p->dia->SetPosition(pos);
        p->dia->Popup();
        e.Skip(); // required to show caret
    });

    auto search_sizer = new wxBoxSizer(wxHORIZONTAL);
    search_sizer->Add(new wxWindow(p->m_search_bar, wxID_ANY, wxDefaultPosition, wxSize(0, 0)), 0, wxEXPAND|wxLEFT|wxRIGHT, FromDIP(1));
    search_sizer->Add(p->m_search_item, 1, wxEXPAND | wxALL | wxALIGN_CENTER_VERTICAL, FromDIP(2));
    p->m_search_bar->SetSizer(search_sizer);
    p->m_search_bar->Layout();
    search_sizer->Fit(p->m_search_bar);

    p->m_object_list = new ObjectList(p->scrolled);
    p->m_object_list->Bind(wxCUSTOMEVT_EXIT_SEARCH, [this](wxCommandEvent&) {
#ifdef __WXGTK__
        this->p->m_search_item->Enable(true);
#endif
        this->p->m_search_bar->SetBorderColor(wxColour("#CECECE"));
        this->p->m_search_item->GetTextCtrl()->SetValue(""); // reset value when close
    });

    p->sizer_params->Add(p->m_search_bar, 0, wxALL | wxEXPAND, 0);
    p->sizer_params->Add(p->m_object_list, 1, wxEXPAND | wxTOP, 0);
    scrolled_sizer->Add(p->sizer_params, 2, wxEXPAND | wxLEFT, 0);
    p->m_object_list->Hide();
    p->m_search_bar->Hide();
    // Frequently Object Settings
    p->object_settings = new ObjectSettings(p->scrolled);

    p->dia = new Search::SearchObjectDialog(p->m_object_list, p->scrolled->GetParent(), p->m_search_item);
#if !NEW_OBJECT_SETTING
    p->object_settings->Hide();
    p->sizer_params->Add(p->object_settings->get_sizer(), 0, wxEXPAND | wxTOP, 5 * em / 10);
#else
    if (params_panel) {
        params_panel->Reparent(p->scrolled);
        scrolled_sizer->Add(params_panel, 3, wxEXPAND);
    }
#endif
    }

    p->object_layers = new ObjectLayers(p->scrolled);
    p->object_layers->Hide();
    p->sizer_params->Add(p->object_layers->get_sizer(), 0, wxEXPAND | wxTOP, 0);

    auto *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(p->scrolled, 1, wxEXPAND);
    SetSizer(sizer);
}

Sidebar::~Sidebar() {}

void Sidebar::create_printer_preset()
{
    CreatePrinterPresetDialog dlg(wxGetApp().mainframe);
    int                       res = dlg.ShowModal();
    if (wxID_OK == res) {
        wxGetApp().mainframe->update_side_preset_ui();
        update_ui_from_settings();
        update_all_preset_comboboxes();
        wxGetApp().load_current_presets();
        CreatePresetSuccessfulDialog success_dlg(wxGetApp().mainframe, SuccessType::PRINTER);
        int                          res = success_dlg.ShowModal();
        if (res == wxID_OK) {
            p->editing_filament = -1;
            if (p->combo_printer->switch_to_tab()) p->editing_filament = 0;
        }
    }
}

void Sidebar::init_filament_combo(PlaterPresetComboBox **combo, const int filament_idx)
{
    *combo = new PlaterPresetComboBox(p->m_panel_scrolled_filament_content, Slic3r::Preset::TYPE_FILAMENT);
    (*combo)->set_filament_idx(filament_idx);

    auto combo_and_btn_sizer = new wxBoxSizer(wxHORIZONTAL);

    // BBS:  filament double columns

    // int em = wxGetApp().em_unit();
    if ((filament_idx % 2) == 0) // Dont add right column item. this one create equal spacing on left, right & middle
        combo_and_btn_sizer->AddSpacer(FromDIP((filament_idx % 2) == 0 ? 12 : 3)); // Content Margin

    (*combo)->clr_picker->SetLabel(wxString::Format("%d", filament_idx + 1));
    combo_and_btn_sizer->Add((*combo)->clr_picker, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(SidebarProps::ElementSpacing()) - FromDIP(2)); // ElementSpacing - 2 (from combo box))
    combo_and_btn_sizer->Add(*combo, 1, wxALL | wxEXPAND, FromDIP(2))->SetMinSize({-1, FromDIP(30)});

    /* BBS hide del_btn
    ScalableButton* del_btn = new ScalableButton(p->m_panel_filament_content, wxID_ANY, "delete_filament");
    del_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& e){
        int extruder_count = std::max(1, (int)p->combos_filament.size() - 1);

        update_objects_list_filament_column(std::max(1, extruder_count - 1));
        on_filaments_change(extruder_count);
        wxGetApp().preset_bundle->printers.get_edited_preset().set_num_extruders(extruder_count);
        wxGetApp().preset_bundle->update_multi_material_filament_presets();
    });

    combo_and_btn_sizer->Add(32 * em / 10, 0, 0, 0, 0);
    combo_and_btn_sizer->Add(del_btn, 0, wxALIGN_CENTER_VERTICAL, 5 * em / 10);
    */
    ScalableButton* edit_btn = new ScalableButton(p->m_panel_scrolled_filament_content, wxID_ANY, "menu_filament");
    edit_btn->SetToolTip(_L("Click to edit preset"));

    PlaterPresetComboBox* combobox = (*combo);
    //edit_btn->Bind(wxEVT_BUTTON, [this, combobox, filament_idx](wxCommandEvent)
    //{
    //    p->editing_filament = -1;
    //    if (combobox->switch_to_tab())
    //        p->editing_filament = filament_idx; // sync with TabPresetComboxBox's m_filament_idx
    //});

    edit_btn->Bind(wxEVT_BUTTON, [this, edit_btn, filament_idx](wxCommandEvent) {
        auto menu = p->plater->filament_action_menu(filament_idx);
        wxPoint pt { 0, edit_btn->GetSize().GetHeight() + 10 };
        pt = edit_btn->ClientToScreen(pt);
        pt = wxGetApp().mainframe->ScreenToClient(pt);
        p->m_menu_filament_id = filament_idx;
        p->plater->PopupMenu(menu, (int) pt.x, pt.y);
    });


    combobox->edit_btn = edit_btn;

    combo_and_btn_sizer->Add(edit_btn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, FromDIP(SidebarProps::ElementSpacing()) - FromDIP(2)); // ElementSpacing - 2 (from combo box))

    combo_and_btn_sizer->AddSpacer(FromDIP(SidebarProps::ContentMargin()));

    // BBS:  filament double columns
    auto side = filament_idx % 2;
    auto /***/sizer_filaments = this->p->sizer_filaments->GetItem(side)->GetSizer();
    if (side == 1 && filament_idx > 1) sizer_filaments->Remove(filament_idx / 2);
    sizer_filaments->Add(combo_and_btn_sizer, 1, wxEXPAND);
    if (side == 0) {
        sizer_filaments = this->p->sizer_filaments->GetItem(1)->GetSizer();
        sizer_filaments->AddStretchSpacer(1);
    }
}

void Sidebar::remove_unused_filament_combos(const size_t current_extruder_count)
{
    if (current_extruder_count >= p->combos_filament.size())
        return;
    while (p->combos_filament.size() > current_extruder_count) {
        const int last = p->combos_filament.size() - 1;
        auto sizer_filaments = this->p->sizer_filaments->GetItem(last % 2)->GetSizer();
        sizer_filaments->Remove(last / 2);
        (*p->combos_filament[last]).Destroy();
        p->combos_filament.pop_back();
    }
    // BBS:  filament double columns
    auto sizer_filaments0 = this->p->sizer_filaments->GetItem((size_t)0)->GetSizer();
    auto sizer_filaments1 = this->p->sizer_filaments->GetItem(1)->GetSizer();
    if (current_extruder_count < 2) {
        sizer_filaments1->Clear();
    } else {
        size_t c0 = sizer_filaments0->GetChildren().GetCount();
        size_t c1 = sizer_filaments1->GetChildren().GetCount();
        if (c0 < c1)
            sizer_filaments1->Remove(c1 - 1);
        else if (c0 > c1)
            sizer_filaments1->AddStretchSpacer(1);
    }
}

void Sidebar::update_all_preset_comboboxes(bool reload_printer_view)
{
    PresetBundle &preset_bundle = *wxGetApp().preset_bundle;
    const auto print_tech = preset_bundle.printers.get_edited_preset().printer_technology();

    bool is_bbl_vendor = preset_bundle.is_bbl_vendor();

    auto p_mainframe = wxGetApp().mainframe;
    auto cfg = preset_bundle.printers.get_edited_preset().config;

    const auto& appconfig = wxGetApp().app_config;

    bool use_new_connection = appconfig->get("use_new_connect") == "true";

    auto printer_config     = wxGetApp().preset_bundle->printers.get_edited_preset().config;
    auto printer_model_opt  = printer_config.option<ConfigOptionString>("printer_model");
    bool is_snapmaker_u1    = false;
    if (printer_model_opt) {
        std::string printer_model = printer_model_opt->value;
        is_snapmaker_u1           = boost::icontains(printer_model, "Snapmaker") && boost::icontains(printer_model, "U1");
    }

    p->combo_printer->set_show_machine_connecting_button(false);
    p->combo_printer->set_show_connection_button(false);

    if (preset_bundle.use_bbl_network()) {
        ams_btn->Show();
        p_mainframe->set_print_button_to_default(MainFrame::PrintSelectType::ePrintPlate);
    } else {
        ams_btn->Hide();
        auto print_btn_type = MainFrame::PrintSelectType::eExportGcode;

        const auto& edit_preset = preset_bundle.printers.get_edited_preset();

        static bool is_sm_page = false;

        if (!use_new_connection && !is_snapmaker_u1 && reload_printer_view) {

            p->combo_printer->set_show_connection_button(true);
            wxString url = cfg.opt_string("print_host_webui").empty() ? cfg.opt_string("print_host") : cfg.opt_string("print_host_webui");
            wxString apikey;
            if (url.empty()) {
                std::string base_url = LOCALHOST_URL + std::to_string(wxGetApp().m_page_http_server.get_port());
                url                  = wxString::Format("%s/web/orca/missing_connection.html", from_u8(base_url));
            }
            else {
                if (!url.Lower().starts_with("http"))
                    url = wxString::Format("http://%s", url);
                const auto host_type = cfg.option<ConfigOptionEnum<PrintHostType>>("host_type")->value;
                if (cfg.has("printhost_apikey") && (host_type != htSimplyPrint))
                    apikey = cfg.opt_string("printhost_apikey");
                print_btn_type = preset_bundle.is_bbl_vendor() ? MainFrame::PrintSelectType::ePrintPlate :
                                                                 MainFrame::PrintSelectType::eSendGcode;

                if (url.find("127.0.0.1") != std::string::npos) {
                    url = wxString::FromUTF8(LOCALHOST_URL + std::to_string(wxGetApp().get_page_http_port()) + "/web/flutter_web/index.html?path=3");
                }
            }
            
            p_mainframe->load_printer_url(url, apikey);
            is_sm_page = false;

            p_mainframe->set_print_button_to_default(print_btn_type);
        } else {
            print_btn_type = preset_bundle.is_bbl_vendor() ? MainFrame::PrintSelectType::ePrintPlate :
                                                             MainFrame::PrintSelectType::eSendGcode;
            p_mainframe->set_print_button_to_default(print_btn_type);

            if (is_snapmaker_u1) {

                auto        devices     = wxGetApp().app_config->get_devices();
                bool hasOnlineMachine = false;
                for (const auto& device : devices) {
                    if (device.connected) {
                        hasOnlineMachine = true;
                        break;
                    }
                }

                if(hasOnlineMachine)
                    p->combo_printer->set_show_machine_connecting_button(true);
    
                wxString url = wxString::FromUTF8(LOCALHOST_URL + std::to_string(wxGetApp().get_page_http_port()) +
                                                  "/web/flutter_web/index.html?path=2");
                auto real_url = wxGetApp().get_international_url(url);
                
                if (!is_sm_page && reload_printer_view) {
                    wxGetApp().mainframe->load_printer_url(real_url); 
                    is_sm_page = true;
                }                   
            }

            if (!p->combo_printer->get_show_machine_connecting_button() && !is_snapmaker_u1) {
                p->combo_printer->set_show_connection_button(true);
            }
        }
    }

    if (cfg.opt_bool("pellet_modded_printer")) {
		p->m_staticText_filament_settings->SetLabel(_L("Pellet Configuration"));
        p->m_filament_icon->SetBitmap_("pellets");
        p->m_filament_config_icon->SetBitmap_("filament_group");
    } else {
		p->m_staticText_filament_settings->SetLabel(_L("Filament Management"));
        p->m_filament_icon->SetBitmap_("filament");
        p->m_filament_config_icon->SetBitmap_("filament_group");
    }

    show_SEMM_buttons(/*cfg.opt_bool("single_extruder_multi_material")*/true);

    bool support_multi_bed_types = cfg.opt_bool("support_multi_bed_types");
    const ConfigOptionDef* bed_type_def = print_config_def.get("curr_bed_type");
    const t_config_enum_values* keys_map = bed_type_def ? bed_type_def->enum_keys_map : nullptr;

    m_bed_type_list->Clear();
    m_bed_type_combo_enum_values.clear();
    if (bed_type_def && keys_map) {
        if (is_snapmaker_u1 && !support_multi_bed_types) {
            for (const auto& item : bed_type_def->enum_labels_u1)
                m_bed_type_list->AppendString(_L(item));
            for (const auto& v : bed_type_def->enum_values_u1)
                m_bed_type_combo_enum_values.push_back(v);
        } else if (is_snapmaker_u1 && support_multi_bed_types) {
            for (const auto& item : bed_type_def->enum_labels_ex)
                m_bed_type_list->AppendString(_L(item));
            for (const auto& v : bed_type_def->enum_values_ex)
                m_bed_type_combo_enum_values.push_back(v);
        } else {
            for (const auto& item : bed_type_def->enum_labels)
                m_bed_type_list->AppendString(_L(item));
            for (const auto& v : bed_type_def->enum_values)
                m_bed_type_combo_enum_values.push_back(v);
        }
    }

    auto get_key_for_bed_type = [keys_map](BedType bt) -> std::string {
        if (!keys_map) return {};
        for (const auto& item : *keys_map)
            if ((BedType)item.second == bt) return item.first;
        return {};
    };
    auto get_selection_index = [&]() -> int {
        BedType curr = wxGetApp().preset_bundle->project_config.opt_enum<BedType>("curr_bed_type");
        std::string key = get_key_for_bed_type(curr);
        for (size_t i = 0; i < m_bed_type_combo_enum_values.size(); ++i)
            if (m_bed_type_combo_enum_values[i] == key) return (int)i;
        return 0;
    };

    if (is_bbl_vendor || support_multi_bed_types || is_snapmaker_u1) {
        m_bed_type_list->Enable();
        // Orca: don't update bed type if loading project
        if (!p->plater->is_loading_project()) {
            std::string printer_name = wxGetApp().preset_bundle->printers.get_selected_preset_name();
            auto str_bed_type = wxGetApp().app_config->get_printer_setting(printer_name, "curr_bed_type");
            
            BedType bed_type_to_use;
            bool is_first_time = str_bed_type.empty();
            
            if (!is_first_time) {
                // This printer has saved bed type configuration
                int bed_type_value = atoi(str_bed_type.c_str());
                if (bed_type_value == 0)
                    bed_type_value = 1;
                bed_type_to_use = (BedType)bed_type_value;
            } else {
                // Orca: First time using this printer, get default bed type
                bed_type_to_use = preset_bundle.printers.get_edited_preset().get_default_bed_type(&preset_bundle);
                
                // Orca: Save to app_config immediately to prevent bed type inheritance
                wxGetApp().app_config->set("curr_bed_type", std::to_string(int(bed_type_to_use)));
                wxGetApp().app_config->set_printer_setting(printer_name, "curr_bed_type", std::to_string(int(bed_type_to_use)));
            }
            
            // Orca: Update proj_config directly to avoid callback context issues
            if (is_snapmaker_u1 && !support_multi_bed_types) {
                if (bed_type_to_use != btPTE && bed_type_to_use != btPEI && bed_type_to_use != btGESP && bed_type_to_use != btSuperTack) {
                    bed_type_to_use = btPTE;
                    wxGetApp().app_config->set("curr_bed_type", std::to_string(int(bed_type_to_use)));
                    wxGetApp().app_config->set_printer_setting(printer_name, "curr_bed_type", std::to_string(int(bed_type_to_use)));
                }
            }

            wxGetApp().preset_bundle->project_config.set_key_value("curr_bed_type", new ConfigOptionEnum<BedType>(bed_type_to_use));
            int sel_idx = get_selection_index();
            m_bed_type_list->SetSelection(sel_idx);
        } else {
            if (is_snapmaker_u1 && !support_multi_bed_types) {
                BedType curr = wxGetApp().preset_bundle->project_config.opt_enum<BedType>("curr_bed_type");
                if (curr != btPTE && curr != btPEI && curr != btGESP && curr != btSuperTack) {
                    wxGetApp().preset_bundle->project_config.set_key_value("curr_bed_type", new ConfigOptionEnum<BedType>(btPTE));
                    m_bed_type_list->SetSelection(0);
                } else
                    m_bed_type_list->SetSelection(get_selection_index());
            } else
                m_bed_type_list->SetSelection(get_selection_index());
        }
    } else {
        BedType default_bed_type = preset_bundle.printers.get_edited_preset().get_default_bed_type(&preset_bundle);
        wxGetApp().preset_bundle->project_config.set_key_value("curr_bed_type", new ConfigOptionEnum<BedType>(default_bed_type));
        m_bed_type_list->SetSelection(get_selection_index());
        m_bed_type_list->Disable();
    }

    if (print_tech == ptFFF) {
        for (PlaterPresetComboBox* cb : p->combos_filament)
            cb->update();
    }

    if (p->combo_printer){
        p->combo_printer->update();
        update_printer_thumbnail();
    }
        
    p_mainframe->show_device(preset_bundle.use_bbl_device_tab() && !use_new_connection);
    p_mainframe->m_tabpanel->SetSelection(p_mainframe->m_tabpanel->GetSelection());
}

void Sidebar::update_presets(Preset::Type preset_type)
{
    PresetBundle &preset_bundle = *wxGetApp().preset_bundle;
    const auto print_tech = preset_bundle.printers.get_edited_preset().printer_technology();

    BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << boost::format(": enter, preset_type %1%")%preset_type;
    switch (preset_type) {
    case Preset::TYPE_FILAMENT:
    {
        // BBS
#if 0
        const size_t extruder_cnt = print_tech != ptFFF ? 1 :
                                dynamic_cast<ConfigOptionFloats*>(preset_bundle.printers.get_edited_preset().config.option("nozzle_diameter"))->values.size();
        const size_t filament_cnt = p->combos_filament.size() > extruder_cnt ? extruder_cnt : p->combos_filament.size();
#else
        const size_t filament_cnt = p->combos_filament.size();
#endif
        const std::string &name = preset_bundle.filaments.get_selected_preset_name();
        if (p->editing_filament >= 0) {
            preset_bundle.set_filament_preset(p->editing_filament, name);
        } else if (filament_cnt == 1) {
            // Single filament printer, synchronize the filament presets.
            Preset *preset = preset_bundle.filaments.find_preset(name, false);
            if (preset) {
                if (preset->is_compatible) preset_bundle.set_filament_preset(0, name);
            }

        }

        for (size_t i = 0; i < filament_cnt; i++)
            p->combos_filament[i]->update();

        update_dynamic_filament_list();

        p->plater->notify_filament_usage_changed();

        break;
    }

    case Preset::TYPE_PRINT:
        //wxGetApp().mainframe->m_param_panel;
        //p->combo_print->update();
        {
        Tab* print_tab = wxGetApp().get_tab(Preset::TYPE_PRINT);
        if (print_tab) {
            print_tab->get_combo_box()->update();
        }
        break;
        }
    case Preset::TYPE_SLA_PRINT:
        ;// p->combo_sla_print->update();
        break;

    case Preset::TYPE_SLA_MATERIAL:
        ;// p->combo_sla_material->update();
        break;

    case Preset::TYPE_PRINTER:
    {
        // update_nozzle_settings();
        auto machineName = wxGetApp().preset_bundle->printers.get_selected_preset_name();

        auto printer_config = wxGetApp().preset_bundle->printers.get_edited_preset().config;
        auto        printer_model_opt = printer_config.option<ConfigOptionString>("printer_model");
        if (printer_model_opt)
        {
            std::string printer_model   = printer_model_opt->value;
            bool        is_snapmaker_u1 = boost::icontains(printer_model, "Snapmaker") && boost::icontains(printer_model, "U1");

            if (is_snapmaker_u1)
            {
                p->m_printerinfo_syncbtn->Show();
            } 
            else 
            {
                p->m_printerinfo_syncbtn->Hide();
            }
        }

        update_all_preset_comboboxes();
        p->show_preset_comboboxes();

        /* update bed shape */
        Tab* printer_tab = wxGetApp().get_tab(Preset::TYPE_PRINTER);
        if (printer_tab) {
            printer_tab->update();
            printer_tab->on_preset_loaded();
        }

        Preset& printer_preset = wxGetApp().preset_bundle->printers.get_edited_preset();
        GLCanvas3D* canvas = wxGetApp().plater()->get_current_canvas3D();
        if (canvas) {
            if (auto printer_structure_opt = printer_preset.config.option<ConfigOptionEnum<PrinterStructure>>("printer_structure")) {
                canvas->get_arrange_settings().align_to_y_axis = (printer_structure_opt->value == PrinterStructure::psI3);
            }
            else
                canvas->get_arrange_settings().align_to_y_axis = false;
        }

        break;
    }

    default: break;
    }

    // Synchronize config.ini with the current selections.
    wxGetApp().preset_bundle->export_selections(*wxGetApp().app_config);

    BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << boost::format(": exit.");

}

//BBS
void Sidebar::update_presets_from_to(Slic3r::Preset::Type preset_type, std::string from, std::string to)
{
    PresetBundle &preset_bundle = *wxGetApp().preset_bundle;

    BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << boost::format(": enter, preset_type %1%, from %2% to %3%")%preset_type %from %to;

    switch (preset_type) {
    case Preset::TYPE_FILAMENT:
    {
        const size_t filament_cnt = p->combos_filament.size();
        for (auto it = preset_bundle.filament_presets.begin(); it != preset_bundle.filament_presets.end(); it++)
        {
            if ((*it).compare(from) == 0) {
                (*it) = to;
            }
        }
        for (size_t i = 0; i < filament_cnt; i++)
            p->combos_filament[i]->update();
        break;
    }

    default: break;
    }

    // Synchronize config.ini with the current selections.
    wxGetApp().preset_bundle->export_selections(*wxGetApp().app_config);

    BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << boost::format(": exit!");
}

void Sidebar::change_top_border_for_mode_sizer(bool increase_border)
{
    // BBS
#if 0
    if (p->mode_sizer) {
        p->mode_sizer->set_items_flag(increase_border ? wxTOP : 0);
        p->mode_sizer->set_items_border(increase_border ? int(0.5 * wxGetApp().em_unit()) : 0);
    }
#endif
}

void Sidebar::msw_rescale()
{
    SetMinSize(wxSize(42 * wxGetApp().em_unit(), -1));
    p->m_panel_printer_title->GetSizer()->SetMinSize(-1, 3 * wxGetApp().em_unit());
    p->m_panel_filament_title->GetSizer()
        ->SetMinSize(-1, 3 * wxGetApp().em_unit());
    p->m_printer_icon->msw_rescale();
    p->m_printerinfo_syncbtn->msw_rescale();
    p->m_printer_setting->msw_rescale();
    p->m_filament_icon->msw_rescale();
    p->m_color_mix_icon->msw_rescale();
    p->m_bpButton_add_filament->msw_rescale();
    p->m_bpButton_del_filament->msw_rescale();
    p->m_bpButton_ams_filament->msw_rescale();
    p->m_bpButton_set_filament->msw_rescale();
    p->m_flushing_volume_btn->Rescale();
    // Batch-match button caches its text extent (messureSize) at creation DPI; without
    // Rescale() the cached size goes stale after a per-monitor DPI change (e.g. moving
    // the window to a 150% 4K screen), so render() centers the auto-rescaled font with
    // the old-DPI metrics and the label drifts/clips.
    p->m_btn_batch_match->Rescale();
    //BBS
    m_bed_type_list->Rescale();
    m_bed_type_list->SetMinSize({-1, 3 * wxGetApp().em_unit()});
#if 0
    if (p->mode_sizer)
        p->mode_sizer->msw_rescale();
#endif

    //for (PlaterPresetComboBox* combo : std::vector<PlaterPresetComboBox*> { p->combo_print,
    //                                                            //p->combo_sla_print,
    //                                                            //p->combo_sla_material,
    //                                                            //p->combo_printer
    //                                                            } )
    //    combo->msw_rescale();
    p->combo_printer->msw_rescale();
    for (PlaterPresetComboBox* combo : p->combos_filament)
        combo->msw_rescale();

    // BBS
    //p->frequently_changed_parameters->msw_rescale();
    //obj_list()->msw_rescale();
    // BBS TODO: add msw_rescale for newly added windows
    // BBS
    //p->object_manipulation->msw_rescale();
    p->object_settings->msw_rescale();
    p->m_search_item->Rescale();
    p->m_search_item->GetTextCtrl()->SetSize(wxSize(-1, FromDIP(16)));
    p->m_search_bar->Layout();

    // BBS
#if 0
    p->object_info->msw_rescale();

    p->btn_send_gcode->msw_rescale();
//    p->btn_eject_device->msw_rescale();
    p->btn_export_gcode_removable->msw_rescale();
#ifdef _WIN32
    const int scaled_height = p->btn_export_gcode_removable->GetBitmapHeight();
#else
    const int scaled_height = p->btn_export_gcode_removable->GetBitmapHeight() + 4;
#endif
    p->btn_export_gcode->SetMinSize(wxSize(-1, scaled_height));
    p->btn_reslice     ->SetMinSize(wxSize(-1, scaled_height));
#endif
    p->scrolled->Layout();

    p->searcher.dlg_msw_rescale();
}

void Sidebar::sys_color_changed()
{
    wxWindowUpdateLocker noUpdates(this);

#if 0
    for (wxWindow* win : std::vector<wxWindow*>{ this, p->sliced_info->GetStaticBox(), p->object_info->GetStaticBox(), p->btn_reslice, p->btn_export_gcode })
        wxGetApp().UpdateDarkUI(win);
    p->object_info->msw_rescale();

    for (wxWindow* win : std::vector<wxWindow*>{ p->scrolled, p->presets_panel })
        wxGetApp().UpdateAllStaticTextDarkUI(win);
#endif
    //for (wxWindow* btn : std::vector<wxWindow*>{ p->btn_reslice, p->btn_export_gcode })
    //    wxGetApp().UpdateDarkUI(btn, true);
    p->m_printer_icon->msw_rescale();
    p->m_printerinfo_syncbtn->msw_rescale();
    p->m_printer_setting->msw_rescale();
    p->m_filament_icon->msw_rescale();
    p->m_color_mix_icon->msw_rescale();
    p->m_bpButton_add_filament->msw_rescale();
    p->m_bpButton_del_filament->msw_rescale();
    p->m_bpButton_ams_filament->msw_rescale();
    p->m_bpButton_set_filament->msw_rescale();
    p->m_flushing_volume_btn->Rescale();
    // Keep the cached text extent in sync with the new DPI, same as above.
    p->m_btn_batch_match->Rescale();

    // BBS
#if 0
    if (p->mode_sizer)
        p->mode_sizer->msw_rescale();
    p->frequently_changed_parameters->sys_color_changed();
#endif
    p->object_settings->sys_color_changed();

    //BBS: remove print related combos
#if 0
    for (PlaterPresetComboBox* combo : std::vector<PlaterPresetComboBox*>{  p->combo_print,
                                                                p->combo_sla_print,
                                                                p->combo_sla_material,
                                                                p->combo_printer })
        combo->sys_color_changed();
#endif
    p->combo_printer->sys_color_changed();
    for (PlaterPresetComboBox* combo : p->combos_filament)
        combo->sys_color_changed();

    // BBS
    obj_list()->sys_color_changed();
    obj_layers()->sys_color_changed();
    // BBS
    //p->object_manipulation->sys_color_changed();

    // btn...->msw_rescale() updates icon on button, so use it
    //p->btn_send_gcode->msw_rescale();
//    p->btn_eject_device->msw_rescale();
    //p->btn_export_gcode_removable->msw_rescale();

    p->scrolled->Layout();

    p->searcher.dlg_sys_color_changed();
}

void Sidebar::search()
{
    p->searcher.search();
}

void Sidebar::jump_to_option(const std::string& opt_key, Preset::Type type, const std::wstring& category)
{
    //const Search::Option& opt = p->searcher.get_option(opt_key, type);
    if (type == Preset::TYPE_PRINT) {
        auto tab = dynamic_cast<TabPrintModel*>(wxGetApp().params_panel()->get_current_tab());
        if (tab && tab->has_key(opt_key)) {
            tab->activate_option(opt_key, category);
            return;
        }
        wxGetApp().params_panel()->switch_to_global();
    }
    wxGetApp().get_tab(type)->activate_option(opt_key, category);
}

void Sidebar::jump_to_option(size_t selected)
{
    const Search::Option& opt = p->searcher.get_option(selected);
    jump_to_option(opt.opt_key(), opt.type, opt.category);

    // Switch to the Settings NotePad
//    wxGetApp().mainframe->select_tab();
}

// BBS. Move logic from Plater::on_extruders_change() to Sidebar::on_filaments_change().
void Sidebar::on_filaments_change(size_t num_filaments)
{
    auto& choices = combos_filament();

    if (num_filaments == choices.size()) {
        // Project load may keep the same physical filament count while mixed
        // definitions changed. Refresh mixed panel even without count changes.
        const bool sync_manager = !p->m_skip_mixed_filament_sync_once;
        p->m_skip_mixed_filament_sync_once = false;
        update_ui_from_settings();
        update_dynamic_filament_list();
        update_mixed_filament_panel(sync_manager);

        // Recalc scrolled filament window height (max 3 rows, matches color mix)
        if (p->m_scrolled_filaments && p->m_panel_scrolled_filament_content) {
            p->m_panel_scrolled_filament_content->Layout();
            const wxSize content_best = p->m_panel_scrolled_filament_content->GetBestSize();
            const int row_count = ((int)num_filaments + 1) / 2; // 2-column grid
            const int desired_h = row_count > 3
                ? (content_best.GetHeight() * 3) / std::max(1, row_count)
                : content_best.GetHeight();
            p->m_scrolled_filaments->SetMinSize({-1, desired_h});
            p->m_scrolled_filaments->SetMaxSize({-1, desired_h});
        }
        Layout();
        return;
    }

    p->m_skip_mixed_filament_sync_once = false;

    if (choices.size() == 1 || num_filaments == 1)
        choices[0]->GetDropDown().Invalidate();

    wxWindowUpdateLocker noUpdates_scrolled_panel(this);

    size_t i = choices.size();
    while (i < num_filaments)
    {
        PlaterPresetComboBox* choice/*{ nullptr }*/;
        init_filament_combo(&choice, i);
        choices.push_back(choice);

        // initialize selection
        choice->update();
        ++i;
    }

    // remove unused choices if any
    remove_unused_filament_combos(num_filaments);

    auto sizer = p->m_panel_filament_title->GetSizer();
    if (p->m_flushing_volume_btn != nullptr && sizer != nullptr) {
        if (num_filaments > 1) {
            sizer->Show(p->m_flushing_volume_btn);
        } else {
            sizer->Hide(p->m_flushing_volume_btn);
        }
    }
    if (p->m_bpButton_del_filament != nullptr && p->m_panel_physical_filaments_title != nullptr) {
        auto* inner_sizer = p->m_panel_physical_filaments_title->GetSizer();
        if (inner_sizer) {
            if (num_filaments > 1)
                inner_sizer->Show(p->m_bpButton_del_filament);
            else
                inner_sizer->Hide(p->m_bpButton_del_filament);
        }
    }

    // Recalc scrolled filament window height (max 3 rows, matches color mix)
    if (p->m_scrolled_filaments && p->m_panel_scrolled_filament_content) {
        p->m_panel_scrolled_filament_content->Layout();
        const wxSize content_best = p->m_panel_scrolled_filament_content->GetBestSize();
        const int row_count = ((int)num_filaments + 1) / 2; // 2-column grid
        const int desired_h = row_count > 3
            ? (content_best.GetHeight() / std::max(1, row_count)) * 3
            : content_best.GetHeight();
        p->m_scrolled_filaments->SetMinSize({-1, desired_h});
        p->m_scrolled_filaments->SetMaxSize({-1, desired_h});
    }

    Layout();
    wxWeakRef<Sidebar> weak_this(this);
    wxTheApp->CallAfter([weak_this]() {
        Sidebar* sidebar = weak_this.get();
        if (sidebar && sidebar->p && sidebar->p->m_scrolled_filaments) {
            int vh = sidebar->p->m_scrolled_filaments->GetVirtualSize().y;
            int ch = sidebar->p->m_scrolled_filaments->GetClientSize().y;
            sidebar->p->m_scrolled_filaments->Scroll(0, std::max(0, vh - ch));
        }
    });
    p->m_panel_filament_title->Refresh();
    update_ui_from_settings();
    update_dynamic_filament_list();
    update_mixed_filament_panel();
    update_color_mix_panel();

    // Disable add buttons when combined filament limit reached
    if (PresetBundle *pb = wxGetApp().preset_bundle) {
        const bool can_add = pb->mixed_filaments.total_filaments(combos_filament().size()) < MAXIMUM_FILAMENT_NUMBER;
        if (p->m_bpButton_add_filament)
            p->m_bpButton_add_filament->Enable(can_add);
        if (p->m_btn_add_color_mix)
            p->m_btn_add_color_mix->Enable(can_add);
    }
}


class MixedGradientWeightsDialog : public wxDialog
{
public:
    MixedGradientWeightsDialog(wxWindow *parent,
                               const std::vector<unsigned int> &filament_ids,
                               const std::vector<wxColour> &palette,
                               const std::vector<int> &initial_weights)
        : wxDialog(parent, wxID_ANY, _L("Gradient Mix Weights"), wxDefaultPosition, wxDefaultSize,
                   wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
    {
        m_colors.reserve(filament_ids.size());
        m_weights = normalize_color_match_weights(initial_weights, filament_ids.size());
        for (const unsigned int filament_id : filament_ids) {
            if (filament_id >= 1 && filament_id <= palette.size())
                m_colors.emplace_back(palette[filament_id - 1]);
            else
                m_colors.emplace_back(wxColour("#26A69A"));
        }
        if (m_colors.empty())
            m_colors.emplace_back(wxColour("#26A69A"));

        auto *root = new wxBoxSizer(wxVERTICAL);
        auto *hint = new wxStaticText(this, wxID_ANY, _L("Pick a point in the gradient map to control multi-filament mix."));
        root->Add(hint, 0, wxEXPAND | wxALL, FromDIP(10));

        m_color_map = new MixedFilamentColorMapPanel(this, filament_ids, palette, initial_weights,
                                                     wxSize(FromDIP(240), FromDIP(240)));
        root->Add(m_color_map, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(10));

        for (size_t i = 0; i < filament_ids.size(); ++i) {
            auto *row = new wxBoxSizer(wxHORIZONTAL);
            wxPanel *chip = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(FromDIP(18), FromDIP(18)), wxBORDER_SIMPLE);
            chip->SetBackgroundColour(m_colors[i]);
            row->Add(chip, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(6));
            row->Add(new wxStaticText(this, wxID_ANY, wxString::Format("F%d", int(filament_ids[i]))),
                     0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(8));
            auto *label = new wxStaticText(this, wxID_ANY, wxString::Format("%d%%", m_weights[i]));
            label->SetFont(Label::Body_12);
            row->Add(label, 0, wxALIGN_CENTER_VERTICAL);
            root->Add(row, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(8));
            m_weight_labels.emplace_back(label);
        }

        root->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, FromDIP(8));
        SetSizerAndFit(root);
        SetMinSize(wxSize(FromDIP(380), std::max(GetSize().GetHeight(), FromDIP(460))));
        update_weight_labels();

        Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent&) { EndModal(wxID_CANCEL); });

        if (m_color_map) {
            m_color_map->Bind(wxEVT_SLIDER, [this](wxCommandEvent &) {
                m_weights = m_color_map ? m_color_map->normalized_weights() : m_weights;
                update_weight_labels();
            });
        }
    }

    std::vector<int> normalized_weights() const
    {
        return m_color_map ? m_color_map->normalized_weights() : m_weights;
    }

private:
    void update_weight_labels()
    {
        for (size_t i = 0; i < m_weight_labels.size() && i < m_weights.size(); ++i) {
            if (m_weight_labels[i])
                m_weight_labels[i]->SetLabel(wxString::Format("%d%%", m_weights[i]));
        }
        Layout();
    }

private:
    MixedFilamentColorMapPanel *m_color_map { nullptr };
    std::vector<wxColour>       m_colors;
    std::vector<int>            m_weights;
    std::vector<wxStaticText*>  m_weight_labels;
};

// Forward declaration for MixedMixPreview (defined below)
class MixedMixPreview;

// Inline editor panel for configuring a single mixed filament
class MixedFilamentConfigPanel : public wxPanel
{
public:
    using OnChangeFn = std::function<void(const MixedFilament &)>;

    MixedFilamentConfigPanel(wxWindow *parent,
                             size_t mixed_id,
                             const MixedFilament &mf,
                             size_t num_physical,
                             const std::vector<std::string> &physical_colors,
                             const std::vector<double> &nozzle_diameters,
                             const std::vector<wxColour> &palette,
                             const MixedFilamentPreviewSettings &preview_settings,
                             bool bias_mode_enabled,
                             OnChangeFn on_change = {});

    // Get the updated mixed filament data
    MixedFilament get_mixed_filament() const { return m_mf; }
    bool has_changes() const { return m_has_changes; }
    static int effective_local_z_preview_mix_b_percent(const MixedFilament &mf,
                                                       const MixedFilamentPreviewSettings &preview_settings);

private:
    void build_ui();
    void update_preview();
    void update_local_z_breakdown();
    void update_component_picker_visuals();

    size_t                          m_mixed_id;
    MixedFilament                   m_mf;
    size_t                          m_num_physical;
    std::vector<std::string>        m_physical_colors;
    std::vector<double>             m_nozzle_diameters;
    std::vector<wxColour>           m_palette;
    MixedFilamentPreviewSettings    m_preview_settings;
    bool                            m_bias_mode_enabled = false;
    bool                            m_has_changes = false;

    wxChoice                       *m_choice_a = nullptr;
    wxChoice                       *m_choice_b = nullptr;
    wxChoice                       *m_choice_c = nullptr;
    wxChoice                       *m_choice_d = nullptr;
    wxPanel                        *m_picker_a_container = nullptr;
    wxPanel                        *m_picker_b_container = nullptr;
    wxPanel                        *m_picker_c_container = nullptr;
    wxPanel                        *m_picker_d_container = nullptr;
    wxPanel                        *m_picker_a_swatch = nullptr;
    wxPanel                        *m_picker_b_swatch = nullptr;
    wxPanel                        *m_picker_c_swatch = nullptr;
    wxPanel                        *m_picker_d_swatch = nullptr;
    wxStaticText                   *m_picker_a_label = nullptr;
    wxStaticText                   *m_picker_b_label = nullptr;
    wxStaticText                   *m_picker_c_label = nullptr;
    wxStaticText                   *m_picker_d_label = nullptr;
    wxPanel                        *m_surface_offset_target_container = nullptr;
    wxPanel                        *m_surface_offset_target_swatch = nullptr;
    wxStaticText                   *m_surface_offset_target_label = nullptr;
    MixedGradientSelector          *m_blend_selector = nullptr;
    wxStaticText                   *m_blend_label = nullptr;
    wxTextCtrl                     *m_pattern_ctrl = nullptr;
    wxCheckBox                     *m_local_z_limit_checkbox = nullptr;
    wxSpinCtrl                     *m_local_z_limit_spin = nullptr;
    wxSpinCtrlDouble               *m_surface_offset_spin = nullptr;
    std::vector<wxButton*>          m_pattern_quick_buttons;
    MixedMixPreview                *m_mix_preview = nullptr;
    wxStaticText                   *m_breakdown_label = nullptr;
    wxPanel                        *m_swatch = nullptr;
    std::shared_ptr<std::vector<int>> m_selected_weight_state;
    OnChangeFn                       m_on_change;

    // Helper functions (copied from update_mixed_filament_panel)
    static std::vector<unsigned int> decode_gradient_ids(const std::string &s);
    static std::string encode_gradient_ids(const std::vector<unsigned int> &ids);
    static std::vector<unsigned int> decode_manual_pattern_ids(const std::string &pattern,
                                                               unsigned int       a,
                                                               unsigned int       b,
                                                               size_t             num_physical,
                                                               size_t             wall_loops = 0);
    static std::vector<int> decode_gradient_weights(const std::string &s, size_t n);
    static std::vector<int> normalize_gradient_weights(const std::vector<int> &w, size_t n);
    static std::string encode_gradient_weights(const std::vector<int> &w);
    static std::vector<unsigned int> build_weighted_pair_sequence(unsigned int a, unsigned int b, int percent_b, bool limit_cycle = false);
    static std::vector<unsigned int> build_weighted_multi_sequence(const std::vector<unsigned int> &ids,
                                                                   const std::vector<int> &weights,
                                                                   size_t max_cycle_limit = 0);
    static std::string summarize_sequence(const std::vector<unsigned int> &seq);
    static std::string summarize_local_z_breakdown(const MixedFilament &mf,
                                                   const std::vector<int> &weights,
                                                   const MixedFilamentPreviewSettings &preview_settings);
    static std::string blend_from_sequence(const std::vector<std::string> &colors, const std::vector<unsigned int> &seq, const std::string &fallback);
    static std::vector<double> build_local_z_preview_pass_heights(double nominal_layer_height,
                                                                  double lower_bound,
                                                                  double upper_bound,
                                                                  double preferred_a_height,
                                                                  double preferred_b_height,
                                                                  int mix_b_percent,
                                                                  int max_sublayers_limit);
};

class MixedMixPreview : public wxPanel
{
public:
    explicit MixedMixPreview(wxWindow *parent)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        SetMinSize(wxSize(FromDIP(120), FromDIP(20)));
        Bind(wxEVT_PAINT, &MixedMixPreview::on_paint, this);
    }

    void set_data(const std::vector<wxColour> &palette,
                  const std::vector<unsigned int> &sequence,
                  bool same_layer_mode,
                  const std::vector<double> &surface_offsets_mm,
                  const wxColour &fallback,
                  const wxString &left_overlay,
                  const wxString &right_overlay)
    {
        m_palette    = palette;
        m_sequence   = sequence;
        m_same_layer = same_layer_mode;
        m_surface_offsets_mm = surface_offsets_mm;
        m_fallback   = fallback;
        m_left_overlay = left_overlay;
        m_right_overlay = right_overlay;
        Refresh();
    }

private:
    wxRect preview_rect() const
    {
        const int margin_x = FromDIP(1);
        const int margin_y = FromDIP(1);
        const wxSize sz = GetClientSize();
        return wxRect(margin_x, margin_y, std::max(1, sz.GetWidth() - margin_x * 2), std::max(1, sz.GetHeight() - margin_y * 2));
    }

    wxColour color_for_extruder(unsigned int extruder_id) const
    {
        if (extruder_id >= 1 && extruder_id <= m_palette.size())
            return m_palette[extruder_id - 1];
        return m_fallback;
    }

    double max_active_surface_offset_mm() const
    {
        double max_offset = 0.0;
        for (double offset_mm : m_surface_offsets_mm)
            max_offset = std::max(max_offset, std::abs(offset_mm));
        return std::max(0.001, max_offset);
    }

    int slot_inset_for_extruder(unsigned int extruder_id, int slot_extent) const
    {
        if (extruder_id == 0 || extruder_id >= m_surface_offsets_mm.size() || slot_extent <= 2)
            return 0;

        const double offset_mm = m_surface_offsets_mm[extruder_id];
        if (std::abs(offset_mm) <= EPSILON)
            return 0;

        const double normalized = std::clamp(std::abs(offset_mm) / max_active_surface_offset_mm(), 0.0, 1.0);
        const int inset = int(std::round(normalized * slot_extent * 0.45)) * (offset_mm < 0.0 ? -1 : 1);
        return std::clamp(inset, -std::max(0, slot_extent / 2), std::max(0, slot_extent / 2));
    }

    void on_paint(wxPaintEvent &)
    {
        wxAutoBufferedPaintDC dc(this);
        dc.SetBackground(wxBrush(GetBackgroundColour()));
        dc.Clear();

        const wxRect rect = preview_rect();
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(m_fallback));
        dc.DrawRectangle(rect);

        if (!m_sequence.empty()) {
            if (m_same_layer) {
                // Same-layer preview: full-height stripe lines.
                const int stripes = 24;
                const int stripe_w = std::max(1, rect.GetWidth() / stripes);
                const size_t seq_len = m_sequence.size();
                for (int s = 0; s < stripes; ++s) {
                    const size_t idx = size_t(s % int(seq_len));
                    const unsigned int extruder_id = m_sequence[idx];
                    dc.SetBrush(wxBrush(color_for_extruder(m_sequence[idx])));
                    const int x = rect.GetLeft() + s * stripe_w;
                    const int w = (s == stripes - 1) ? (rect.GetRight() - x + 1) : stripe_w;
                    const int inset = slot_inset_for_extruder(extruder_id, w);
                    wxRect draw_rect(x + inset / 2, rect.GetTop(), std::max(1, w - inset), rect.GetHeight());
                    draw_rect.Intersect(rect);
                    if (draw_rect.GetWidth() > 0)
                        dc.DrawRectangle(draw_rect);
                }
            } else {
                const int bars = 24;
                const int bar_w = std::max(1, rect.GetWidth() / bars);
                for (int i = 0; i < bars; ++i) {
                    size_t idx = 0;
                    if (m_sequence.size() > size_t(bars))
                        idx = (size_t(i) * m_sequence.size()) / size_t(bars);
                    else
                        idx = size_t(i) % m_sequence.size();
                    const unsigned int extruder_id = m_sequence[idx];
                    dc.SetBrush(wxBrush(color_for_extruder(extruder_id)));
                    const int x = rect.GetLeft() + i * bar_w;
                    const int w = (i == bars - 1) ? (rect.GetRight() - x + 1) : bar_w;
                    const int inset = slot_inset_for_extruder(extruder_id, w);
                    wxRect draw_rect(x + inset / 2, rect.GetTop(), std::max(1, w - inset), rect.GetHeight());
                    draw_rect.Intersect(rect);
                    if (draw_rect.GetWidth() > 0)
                        dc.DrawRectangle(draw_rect);
                }
            }
        }

        auto draw_outlined_text = [this, &dc](const wxString &text, int x, int y) {
            if (text.empty())
                return;
            dc.SetTextForeground(wxColour(255, 255, 255));
            const int outline_radius = std::max(2, FromDIP(2));
            for (int ox = -outline_radius; ox <= outline_radius; ++ox) {
                for (int oy = -outline_radius; oy <= outline_radius; ++oy) {
                    if (ox == 0 && oy == 0)
                        continue;
                    dc.DrawText(text, x + ox, y + oy);
                }
            }
            dc.SetTextForeground(wxColour(22, 22, 22));
            dc.DrawText(text, x, y);
        };

        wxCoord left_w = 0, left_h = 0;
        wxCoord right_w = 0, right_h = 0;
        dc.GetTextExtent(m_left_overlay, &left_w, &left_h);
        dc.GetTextExtent(m_right_overlay, &right_w, &right_h);
        const int text_y = rect.GetTop() + std::max(0, (rect.GetHeight() - int(std::max(left_h, right_h))) / 2);
        const int pad = FromDIP(6);
        if (!m_left_overlay.empty())
            draw_outlined_text(m_left_overlay, rect.GetLeft() + pad, text_y);
        if (!m_right_overlay.empty())
            draw_outlined_text(m_right_overlay, rect.GetRight() - pad - int(right_w), text_y);

        const bool is_dark = wxGetApp().dark_mode();
        dc.SetPen(wxPen(is_dark ? wxColour(110, 110, 110) : wxColour(170, 170, 170), 1));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(rect);
    }

private:
    std::vector<wxColour>       m_palette;
    std::vector<unsigned int>   m_sequence;
    std::vector<double>         m_surface_offsets_mm;
    bool                        m_same_layer { false };
    wxColour                    m_fallback { wxColour(38, 166, 154) };
    wxString                    m_left_overlay;
    wxString                    m_right_overlay;
};

// Implementation of MixedFilamentConfigPanel helper functions
static std::vector<unsigned int> build_grouped_manual_pattern_preview_sequence(const std::string &pattern,
                                                                               unsigned int       component_a,
                                                                               unsigned int       component_b,
                                                                               size_t             num_physical,
                                                                               size_t             wall_loops)
{
    std::vector<unsigned int> sequence;
    if (num_physical == 0)
        return sequence;

    const std::string normalized = MixedFilamentManager::normalize_manual_pattern(pattern);
    if (normalized.empty())
        return sequence;

    const std::vector<std::string> groups = MixedFilamentManager::split_pattern_groups(normalized);
    if (groups.empty())
        return sequence;

    MixedFilament dummy_mf;
    dummy_mf.component_a = component_a;
    dummy_mf.component_b = component_b;

    if (groups.size() == 1) {
        const std::vector<std::string> tokens =
            MixedFilamentManager::split_pattern_group_to_tokens(groups[0], num_physical);
        sequence.reserve(tokens.size());
        for (const std::string &token : tokens) {
            const unsigned int extruder_id =
                MixedFilamentManager::physical_filament_from_token(token, dummy_mf, num_physical);
            if (extruder_id != 0)
                sequence.emplace_back(extruder_id);
        }
        return sequence;
    }

    std::vector<std::vector<std::string>> group_tokens;
    group_tokens.reserve(groups.size());
    for (const std::string &group : groups)
        group_tokens.push_back(MixedFilamentManager::split_pattern_group_to_tokens(group, num_physical));

    constexpr size_t k_max_preview_cycle = 48;
    size_t cycle = 1;
    for (const auto &tokens : group_tokens) {
        if (tokens.empty())
            continue;
        cycle = std::lcm(cycle, tokens.size());
        if (cycle >= k_max_preview_cycle) {
            cycle = k_max_preview_cycle;
            break;
        }
    }

    const size_t preview_wall_loops = std::max<size_t>(1, wall_loops == 0 ? groups.size() : wall_loops);
    sequence.reserve(preview_wall_loops * cycle);
    for (size_t layer_idx = 0; layer_idx < cycle; ++layer_idx) {
        for (size_t wall_idx = 0; wall_idx < preview_wall_loops; ++wall_idx) {
            const auto &tokens = group_tokens[std::min(wall_idx, group_tokens.size() - 1)];
            if (tokens.empty())
                continue;
            const std::string &token = tokens[layer_idx % tokens.size()];
            const unsigned int extruder_id =
                MixedFilamentManager::physical_filament_from_token(token, dummy_mf, num_physical);
            if (extruder_id != 0)
                sequence.emplace_back(extruder_id);
        }
    }

    return sequence;
}

std::vector<unsigned int> MixedFilamentConfigPanel::decode_gradient_ids(const std::string &s)
{
    return MixedFilamentManager::decode_gradient_component_ids(s, 0);
}

std::string MixedFilamentConfigPanel::encode_gradient_ids(const std::vector<unsigned int> &ids)
{
    return MixedFilamentManager::encode_gradient_component_ids(ids);
}

std::vector<unsigned int> MixedFilamentConfigPanel::decode_manual_pattern_ids(const std::string &pattern,
                                                                              unsigned int       a,
                                                                              unsigned int       b,
                                                                              size_t             num_physical,
                                                                              size_t             wall_loops)
{
    return build_grouped_manual_pattern_preview_sequence(pattern, a, b, num_physical, wall_loops);
}

std::vector<int> MixedFilamentConfigPanel::decode_gradient_weights(const std::string &s, size_t n)
{
    std::vector<int> w;
    if (s.empty() || n == 0)
        return w;

    std::string token;
    for (const char c : s) {
        if (c >= '0' && c <= '9') {
            token.push_back(c);
            continue;
        }
        if (!token.empty()) {
            w.emplace_back(std::max(0, std::atoi(token.c_str())));
            token.clear();
        }
    }
    if (!token.empty())
        w.emplace_back(std::max(0, std::atoi(token.c_str())));
    if (w.size() != n)
        w.clear();
    return w;
}

std::vector<int> MixedFilamentConfigPanel::normalize_gradient_weights(const std::vector<int> &w, size_t n)
{
    std::vector<int> out = w;
    if (out.size() != n) out.assign(n, n > 0 ? int(100 / n) : 0);
    int sum = 0;
    for (int &v : out) { v = std::max(0, v); sum += v; }
    if (sum <= 0 && n > 0) { out.assign(n, 0); out[0] = 100; return out; }
    std::vector<double> rem(n, 0.);
    int assigned = 0;
    for (size_t i = 0; i < n; ++i) {
        const double exact = 100.0 * double(out[i]) / double(sum);
        out[i] = int(std::floor(exact));
        rem[i] = exact - double(out[i]);
        assigned += out[i];
    }
    int missing = std::max(0, 100 - assigned);
    while (missing > 0) {
        size_t best = 0;
        double best_rem = -1.0;
        for (size_t i = 0; i < rem.size(); ++i) {
            if (rem[i] > best_rem) { best_rem = rem[i]; best = i; }
        }
        ++out[best];
        rem[best] = 0.0;
        --missing;
    }
    return out;
}

std::string MixedFilamentConfigPanel::encode_gradient_weights(const std::vector<int> &w)
{
    std::ostringstream out;
    for (size_t i = 0; i < w.size(); ++i) {
        if (i > 0)
            out << '/';
        out << std::max(0, w[i]);
    }
    return out.str();
}

namespace {

std::pair<int, int> effective_pair_preview_ratios(int percent_b)
{
    const int mix_b = std::clamp(percent_b, 0, 100);
    int       ratio_a = 1;
    int       ratio_b = 0;

    if (mix_b >= 100) {
        ratio_a = 0;
        ratio_b = 1;
    } else if (mix_b > 0) {
        const int pct_b      = mix_b;
        const int pct_a      = 100 - pct_b;
        const bool b_is_major = pct_b >= pct_a;
        const int major_pct  = b_is_major ? pct_b : pct_a;
        const int minor_pct  = b_is_major ? pct_a : pct_b;
        const int major_layers =
            std::max(1, int(std::lround(double(major_pct) / double(std::max(1, minor_pct)))));
        ratio_a = b_is_major ? 1 : major_layers;
        ratio_b = b_is_major ? major_layers : 1;
    }

    if (ratio_a > 0 && ratio_b > 0) {
        const int g = std::gcd(ratio_a, ratio_b);
        if (g > 1) {
            ratio_a /= g;
            ratio_b /= g;
        }
    }

    return { std::max(0, ratio_a), std::max(0, ratio_b) };
}

std::vector<unsigned int> build_effective_pair_preview_sequence(unsigned int component_a,
                                                                unsigned int component_b,
                                                                int          percent_b,
                                                                bool         limit_cycle)
{
    std::vector<unsigned int> sequence;
    if (component_a == 0 || component_b == 0 || component_a == component_b)
        return sequence;

    auto [ratio_a, ratio_b] = effective_pair_preview_ratios(percent_b);
    constexpr int k_max_cycle = 24;
    if (limit_cycle && ratio_a > 0 && ratio_b > 0 && ratio_a + ratio_b > k_max_cycle) {
        const double scale = double(k_max_cycle) / double(ratio_a + ratio_b);
        ratio_a = std::max(1, int(std::round(double(ratio_a) * scale)));
        ratio_b = std::max(1, int(std::round(double(ratio_b) * scale)));
    }
    if (ratio_a == 0 && ratio_b == 0)
        ratio_a = 1;

    const int cycle = std::max(1, ratio_a + ratio_b);
    sequence.reserve(size_t(cycle));
    for (int pos = 0; pos < cycle; ++pos) {
        const int b_before = (pos * ratio_b) / cycle;
        const int b_after  = ((pos + 1) * ratio_b) / cycle;
        sequence.emplace_back((b_after > b_before) ? component_b : component_a);
    }
    return sequence;
}

std::string format_preview_sequence_percent(int count, int total)
{
    if (count <= 0 || total <= 0)
        return "";

    const double percent         = 100.0 * double(count) / double(total);
    const double rounded_tenths  = std::round(percent * 10.0) / 10.0;
    const double nearest_integer = std::round(rounded_tenths);
    if (std::abs(rounded_tenths - nearest_integer) < 1e-6)
        return wxString::Format("%d%%", int(nearest_integer)).ToStdString();
    return wxString::Format("%.1f%%", rounded_tenths).ToStdString();
}

} // namespace

std::vector<unsigned int> MixedFilamentConfigPanel::build_weighted_pair_sequence(unsigned int a,
                                                                                 unsigned int b,
                                                                                 int          percent_b,
                                                                                 bool         limit_cycle)
{
    return build_effective_pair_preview_sequence(a, b, percent_b, limit_cycle);
}

static void reduce_weight_counts_to_cycle_limit(std::vector<int> &counts, size_t cycle_limit)
{
    if (counts.empty() || cycle_limit == 0)
        return;

    int total = std::accumulate(counts.begin(), counts.end(), 0);
    if (total <= 0 || size_t(total) <= cycle_limit)
        return;

    std::vector<size_t> positive_indices;
    positive_indices.reserve(counts.size());
    for (size_t i = 0; i < counts.size(); ++i)
        if (counts[i] > 0)
            positive_indices.emplace_back(i);

    if (positive_indices.empty()) {
        counts.assign(counts.size(), 0);
        return;
    }

    std::vector<int> reduced(counts.size(), 0);
    if (cycle_limit < positive_indices.size()) {
        std::sort(positive_indices.begin(), positive_indices.end(), [&counts](size_t lhs, size_t rhs) {
            if (counts[lhs] != counts[rhs])
                return counts[lhs] > counts[rhs];
            return lhs < rhs;
        });
        for (size_t i = 0; i < cycle_limit; ++i)
            reduced[positive_indices[i]] = 1;
        counts = std::move(reduced);
        return;
    }

    size_t remaining_slots = cycle_limit;
    for (const size_t idx : positive_indices) {
        reduced[idx] = 1;
        --remaining_slots;
    }

    int total_extras = 0;
    std::vector<int> extra_counts(counts.size(), 0);
    for (const size_t idx : positive_indices) {
        extra_counts[idx] = std::max(0, counts[idx] - 1);
        total_extras += extra_counts[idx];
    }
    if (remaining_slots == 0 || total_extras <= 0) {
        counts = std::move(reduced);
        return;
    }

    std::vector<double> remainders(counts.size(), -1.0);
    size_t assigned_slots = 0;
    for (const size_t idx : positive_indices) {
        if (extra_counts[idx] == 0)
            continue;
        const double exact = double(remaining_slots) * double(extra_counts[idx]) / double(total_extras);
        const int assigned = int(std::floor(exact));
        reduced[idx] += assigned;
        assigned_slots += size_t(assigned);
        remainders[idx] = exact - double(assigned);
    }

    size_t missing_slots = remaining_slots > assigned_slots ? (remaining_slots - assigned_slots) : size_t(0);
    while (missing_slots > 0) {
        size_t best_idx = size_t(-1);
        double best_remainder = -1.0;
        int    best_extra = -1;
        for (const size_t idx : positive_indices) {
            if (extra_counts[idx] == 0)
                continue;
            if (remainders[idx] > best_remainder ||
                (std::abs(remainders[idx] - best_remainder) <= 1e-9 && extra_counts[idx] > best_extra) ||
                (std::abs(remainders[idx] - best_remainder) <= 1e-9 && extra_counts[idx] == best_extra && idx < best_idx)) {
                best_idx = idx;
                best_remainder = remainders[idx];
                best_extra = extra_counts[idx];
            }
        }
        if (best_idx == size_t(-1))
            break;
        ++reduced[best_idx];
        remainders[best_idx] = -1.0;
        --missing_slots;
    }

    counts = std::move(reduced);
}

std::vector<unsigned int> MixedFilamentConfigPanel::build_weighted_multi_sequence(const std::vector<unsigned int> &ids,
                                                                                  const std::vector<int> &weights,
                                                                                  size_t max_cycle_limit)
{
    std::vector<unsigned int> seq;
    if (ids.empty())
        return seq;

    std::vector<unsigned int> filtered_ids;
    std::vector<int> counts;
    filtered_ids.reserve(ids.size());
    counts.reserve(ids.size());

    std::vector<int> normalized = normalize_gradient_weights(weights, ids.size());
    for (size_t i = 0; i < ids.size(); ++i) {
        const int weight = (i < normalized.size()) ? std::max(0, normalized[i]) : 0;
        if (weight <= 0)
            continue;
        filtered_ids.emplace_back(ids[i]);
        counts.emplace_back(weight);
    }
    if (filtered_ids.empty()) {
        filtered_ids = ids;
        counts.assign(ids.size(), 1);
    }

    int g = 0;
    for (const int c : counts)
        g = std::gcd(g, std::max(1, c));
    if (g > 1) {
        for (int &c : counts)
            c = std::max(1, c / g);
    }

    constexpr size_t k_max_cycle = 48;
    const size_t effective_cycle_limit =
        max_cycle_limit > 0 ? std::min(k_max_cycle, std::max<size_t>(1, max_cycle_limit)) : k_max_cycle;
    reduce_weight_counts_to_cycle_limit(counts, effective_cycle_limit);

    std::vector<unsigned int> reduced_ids;
    std::vector<int> reduced_counts;
    reduced_ids.reserve(filtered_ids.size());
    reduced_counts.reserve(counts.size());
    for (size_t i = 0; i < counts.size(); ++i) {
        if (counts[i] <= 0)
            continue;
        reduced_ids.emplace_back(filtered_ids[i]);
        reduced_counts.emplace_back(counts[i]);
    }
    if (reduced_ids.empty())
        return seq;
    filtered_ids = std::move(reduced_ids);
    counts = std::move(reduced_counts);

    const int total = std::accumulate(counts.begin(), counts.end(), 0);
    if (total <= 0)
        return seq;

    const size_t cycle = size_t(total);

    seq.reserve(cycle);
    std::vector<int> emitted(counts.size(), 0);
    for (size_t pos = 0; pos < cycle; ++pos) {
        size_t best_idx = 0;
        double best_score = -1e9;
        for (size_t i = 0; i < counts.size(); ++i) {
            const double target = double(pos + 1) * double(counts[i]) / double(total);
            const double score = target - double(emitted[i]);
            if (score > best_score) {
                best_score = score;
                best_idx = i;
            }
        }
        ++emitted[best_idx];
        seq.emplace_back(filtered_ids[best_idx]);
    }
    if (seq.empty())
        seq = filtered_ids;
    return seq;
}


std::vector<double> MixedFilamentConfigPanel::build_local_z_preview_pass_heights(double nominal_layer_height,
                                                                                 double lower_bound,
                                                                                 double upper_bound,
                                                                                 double preferred_a_height,
                                                                                 double preferred_b_height,
                                                                                 int mix_b_percent,
                                                                                 int max_sublayers_limit)
{
    if (nominal_layer_height <= EPSILON)
        return {};

    const double base_height = nominal_layer_height;
    const double lo = std::max<double>(0.01, lower_bound);
    const double hi = std::max<double>(lo, upper_bound);
    const size_t max_passes_limit = max_sublayers_limit >= 2 ? size_t(max_sublayers_limit) : size_t(0);

    auto fit_pass_heights_to_interval = [](std::vector<double> &passes, double total_height, double local_lo, double local_hi) {
        if (passes.empty() || total_height <= EPSILON)
            return false;

        const auto within = [local_lo, local_hi](double value) {
            return value >= local_lo - 1e-6 && value <= local_hi + 1e-6;
        };

        double sum = 0.0;
        for (const double h : passes)
            sum += h;

        double delta = total_height - sum;
        if (std::abs(delta) > 1e-6) {
            if (delta > 0.0) {
                for (double &h : passes) {
                    if (delta <= 1e-6)
                        break;
                    const double room = local_hi - h;
                    if (room <= 1e-6)
                        continue;
                    const double take = std::min(room, delta);
                    h += take;
                    delta -= take;
                }
            } else {
                for (auto it = passes.rbegin(); it != passes.rend() && delta < -1e-6; ++it) {
                    const double room = *it - local_lo;
                    if (room <= 1e-6)
                        continue;
                    const double take = std::min(room, -delta);
                    *it -= take;
                    delta += take;
                }
            }
        }

        if (std::abs(delta) > 1e-6)
            return false;
        return std::all_of(passes.begin(), passes.end(), within);
    };

    auto build_uniform = [&fit_pass_heights_to_interval, base_height, lo, hi, max_passes_limit]() {
        std::vector<double> out;
        size_t min_passes = size_t(std::max<double>(1.0, std::ceil((base_height - EPSILON) / hi)));
        size_t max_passes = size_t(std::max<double>(1.0, std::floor((base_height + EPSILON) / lo)));
        size_t pass_count = min_passes;

        if (max_passes >= min_passes) {
            const double target_step = 0.5 * (lo + hi);
            const size_t target_passes =
                size_t(std::max<double>(1.0, std::llround(base_height / std::max<double>(target_step, EPSILON))));
            pass_count = std::clamp(target_passes, min_passes, max_passes);
        }

        if (max_passes_limit > 0 && pass_count > max_passes_limit)
            pass_count = max_passes_limit;

        if (pass_count == 1 && base_height >= 2.0 * lo - EPSILON && max_passes >= 2)
            pass_count = 2;

        if (pass_count <= 1) {
            out.emplace_back(base_height);
            return out;
        }

        out.assign(pass_count, base_height / double(pass_count));
        double accumulated = 0.0;
        for (size_t i = 0; i + 1 < out.size(); ++i)
            accumulated += out[i];
        out.back() = std::max<double>(EPSILON, base_height - accumulated);
        if (!fit_pass_heights_to_interval(out, base_height, lo, hi) && max_passes_limit == 0) {
            out.assign(pass_count, base_height / double(pass_count));
            accumulated = 0.0;
            for (size_t i = 0; i + 1 < out.size(); ++i)
                accumulated += out[i];
            out.back() = std::max<double>(EPSILON, base_height - accumulated);
        }
        return out;
    };

    auto build_alternating = [&build_uniform, &fit_pass_heights_to_interval, base_height, lo, hi, max_passes_limit](double gradient_h_a, double gradient_h_b) {
        if (base_height < 2.0 * lo - EPSILON)
            return std::vector<double>{ base_height };

        const double cycle_h = std::max<double>(EPSILON, gradient_h_a + gradient_h_b);
        const double ratio_a = std::clamp(gradient_h_a / cycle_h, 0.0, 1.0);

        size_t min_passes = size_t(std::max<double>(2.0, std::ceil((base_height - EPSILON) / hi)));
        if ((min_passes % 2) != 0)
            ++min_passes;

        size_t max_passes = size_t(std::max<double>(2.0, std::floor((base_height + EPSILON) / lo)));
        if ((max_passes % 2) != 0)
            --max_passes;
        if (max_passes_limit > 0) {
            size_t capped_limit = std::max<size_t>(2, max_passes_limit);
            if ((capped_limit % 2) != 0)
                --capped_limit;
            if (capped_limit >= 2)
                max_passes = std::min(max_passes, capped_limit);
        }
        if (max_passes < 2)
            return build_uniform();
        if (min_passes > max_passes)
            min_passes = max_passes;
        if (min_passes < 2)
            min_passes = 2;
        if ((min_passes % 2) != 0)
            ++min_passes;
        if (min_passes > max_passes)
            return build_uniform();

        const double target_step = 0.5 * (lo + hi);
        size_t target_passes =
            size_t(std::max<double>(2.0, std::llround(base_height / std::max<double>(target_step, EPSILON))));
        if ((target_passes % 2) != 0) {
            const size_t round_up = (target_passes < max_passes) ? (target_passes + 1) : max_passes;
            const size_t round_down = (target_passes > min_passes) ? (target_passes - 1) : min_passes;
            if (round_up > max_passes)
                target_passes = round_down;
            else if (round_down < min_passes)
                target_passes = round_up;
            else
                target_passes = ((round_up - target_passes) <= (target_passes - round_down)) ? round_up : round_down;
        }
        target_passes = std::clamp(target_passes, min_passes, max_passes);

        bool                has_best           = false;
        std::vector<double> best_passes;
        double              best_ratio_error   = 0.0;
        size_t              best_pass_distance = 0;
        double              best_max_height    = 0.0;
        size_t              best_pass_count    = 0;

        for (size_t pass_count = min_passes; pass_count <= max_passes; pass_count += 2) {
            const size_t pair_count = pass_count / 2;
            if (pair_count == 0)
                continue;
            const double pair_h = base_height / double(pair_count);

            const double h_a_min = std::max(lo, pair_h - hi);
            const double h_a_max = std::min(hi, pair_h - lo);
            if (h_a_min > h_a_max + EPSILON)
                continue;

            const double h_a = std::clamp(pair_h * ratio_a, h_a_min, h_a_max);
            const double h_b = pair_h - h_a;

            std::vector<double> out;
            out.reserve(pass_count);
            for (size_t pair_idx = 0; pair_idx < pair_count; ++pair_idx) {
                out.emplace_back(h_a);
                out.emplace_back(h_b);
            }
            if (!fit_pass_heights_to_interval(out, base_height, lo, hi))
                continue;

            const double ratio_actual = (h_a + h_b > EPSILON) ? (h_a / (h_a + h_b)) : 0.5;
            const double ratio_error  = std::abs(ratio_actual - ratio_a);
            const size_t pass_distance =
                (pass_count > target_passes) ? (pass_count - target_passes) : (target_passes - pass_count);
            const double max_height = std::max(h_a, h_b);

            const bool better_ratio         = !has_best || (ratio_error + 1e-6 < best_ratio_error);
            const bool similar_ratio        = has_best && std::abs(ratio_error - best_ratio_error) <= 1e-6;
            const bool better_distance      = similar_ratio && (pass_distance < best_pass_distance);
            const bool similar_distance     = similar_ratio && (pass_distance == best_pass_distance);
            const bool better_max_height    = similar_distance && (max_height + 1e-6 < best_max_height);
            const bool similar_max_height   = similar_distance && std::abs(max_height - best_max_height) <= 1e-6;
            const bool better_pass_count    = similar_max_height && (pass_count > best_pass_count);

            if (better_ratio || better_distance || better_max_height || better_pass_count) {
                has_best = true;
                best_passes = std::move(out);
                best_ratio_error = ratio_error;
                best_pass_distance = pass_distance;
                best_max_height = max_height;
                best_pass_count = pass_count;
            }
        }

        return has_best ? best_passes : build_uniform();
    };

    if (preferred_a_height > EPSILON || preferred_b_height > EPSILON) {
        std::vector<double> cadence_unit;
        if (preferred_a_height > EPSILON)
            cadence_unit.push_back(std::clamp(preferred_a_height, lo, hi));
        if (preferred_b_height > EPSILON)
            cadence_unit.push_back(std::clamp(preferred_b_height, lo, hi));

        if (!cadence_unit.empty()) {
            std::vector<double> out;
            out.reserve(size_t(std::ceil(base_height / lo)) + 2);

            double z_used = 0.0;
            size_t idx = 0;
            size_t guard = 0;
            while (z_used + cadence_unit[idx] < base_height - EPSILON && guard++ < 100000) {
                out.push_back(cadence_unit[idx]);
                z_used += cadence_unit[idx];
                idx = (idx + 1) % cadence_unit.size();
            }

            const double remainder = base_height - z_used;
            if (remainder > EPSILON)
                out.push_back(remainder);

            if (fit_pass_heights_to_interval(out, base_height, lo, hi) &&
                (max_passes_limit == 0 || out.size() <= max_passes_limit))
                return out;
        }

        if (preferred_a_height > EPSILON && preferred_b_height > EPSILON)
            return build_alternating(preferred_a_height, preferred_b_height);
        return build_uniform();
    }

    const int mix_b = std::clamp(mix_b_percent, 0, 100);
    const double pct_b = double(mix_b) / 100.0;
    const double pct_a = 1.0 - pct_b;
    const double gradient_h_a = lo + pct_a * (hi - lo);
    const double gradient_h_b = lo + pct_b * (hi - lo);
    return build_alternating(gradient_h_a, gradient_h_b);
}

int MixedFilamentConfigPanel::effective_local_z_preview_mix_b_percent(const MixedFilament &mf,
                                                                      const MixedFilamentPreviewSettings &preview_settings)
{
    return Slic3r::mixed_filament_effective_local_z_preview_mix_b_percent(mf, preview_settings);
}

static double mixed_filament_reference_nozzle_mm(unsigned int               component_a,
                                                 unsigned int               component_b,
                                                 const std::vector<double> &nozzle_diameters)
{
    std::vector<double> samples;
    samples.reserve(2);

    auto append_if_valid = [&samples, &nozzle_diameters](unsigned int component_id) {
        if (component_id >= 1 && component_id <= nozzle_diameters.size())
            samples.emplace_back(std::max(0.05, nozzle_diameters[size_t(component_id - 1)]));
    };

    append_if_valid(component_a);
    append_if_valid(component_b);

    if (samples.empty())
        return 0.4;
    return std::accumulate(samples.begin(), samples.end(), 0.0) / double(samples.size());
}

static double mixed_filament_bias_limit_mm(const MixedFilament &mf, const std::vector<double> &nozzle_diameters)
{
    const double reference_nozzle_mm = mixed_filament_reference_nozzle_mm(mf.component_a, mf.component_b, nozzle_diameters);
    return MixedFilamentManager::max_pair_bias_mm(float(reference_nozzle_mm));
}

static float mixed_filament_single_surface_offset_value(const MixedFilament       &mf,
                                                        const std::vector<double> &nozzle_diameters)
{
    const double reference_nozzle_mm = mixed_filament_reference_nozzle_mm(mf.component_a, mf.component_b, nozzle_diameters);
    return MixedFilamentManager::bias_ui_value_from_surface_offsets(
        mf.component_a_surface_offset,
        mf.component_b_surface_offset,
        float(reference_nozzle_mm));
}

static std::pair<float, float> mixed_filament_single_surface_offset_pair(const MixedFilament       &mf,
                                                                         float                      value,
                                                                         const std::vector<double> &nozzle_diameters)
{
    const double reference_nozzle_mm = mixed_filament_reference_nozzle_mm(mf.component_a, mf.component_b, nozzle_diameters);
    return MixedFilamentManager::surface_offset_pair_from_signed_bias(value, float(reference_nozzle_mm));
}

static std::string mixed_filament_apparent_pair_summary(const MixedFilament               &mf,
                                                        const MixedFilamentPreviewSettings &preview_settings,
                                                        const std::vector<double>          &nozzle_diameters,
                                                        bool                                bias_mode_enabled)
{
    if (!Slic3r::mixed_filament_supports_bias_apparent_color(mf, preview_settings, bias_mode_enabled))
        return {};

    const int base_b = MixedFilamentConfigPanel::effective_local_z_preview_mix_b_percent(mf, preview_settings);
    const int base_a = 100 - base_b;
    const auto [apparent_a, apparent_b] =
        Slic3r::mixed_filament_apparent_pair_percentages(mf, preview_settings, nozzle_diameters, bias_mode_enabled);

    if (std::abs(mf.component_a_surface_offset - mf.component_b_surface_offset) > 1e-4f &&
        (apparent_a != base_a || apparent_b != base_b)) {
        std::ostringstream ss;
        ss << '~' << apparent_a << '/' << apparent_b;
        return ss.str();
    }

    std::ostringstream ss;
    ss << apparent_a << "%/" << apparent_b << '%';
    return ss.str();
}


static std::vector<unsigned int> build_display_weighted_multi_sequence(const std::vector<unsigned int> &ids,
                                                                       const std::vector<int>          &weights,
                                                                       size_t                           max_cycle_limit = 0)
{
    if (ids.empty())
        return {};

    std::vector<unsigned int> filtered_ids;
    std::vector<int>          counts;
    filtered_ids.reserve(ids.size());
    counts.reserve(ids.size());

    const std::vector<int> normalized = normalize_color_match_weights(weights, ids.size());
    for (size_t idx = 0; idx < ids.size(); ++idx) {
        const int weight = idx < normalized.size() ? std::max(0, normalized[idx]) : 0;
        if (weight <= 0)
            continue;
        filtered_ids.emplace_back(ids[idx]);
        counts.emplace_back(weight);
    }
    if (filtered_ids.empty()) {
        filtered_ids = ids;
        counts.assign(ids.size(), 1);
    }

    int g = 0;
    for (const int count : counts)
        g = std::gcd(g, std::max(1, count));
    if (g > 1) {
        for (int &count : counts)
            count = std::max(1, count / g);
    }

    constexpr size_t k_max_cycle = 48;
    const size_t effective_cycle_limit =
        max_cycle_limit > 0 ? std::min(k_max_cycle, std::max<size_t>(1, max_cycle_limit)) : k_max_cycle;
    reduce_weight_counts_to_cycle_limit(counts, effective_cycle_limit);

    std::vector<unsigned int> reduced_ids;
    std::vector<int>          reduced_counts;
    reduced_ids.reserve(filtered_ids.size());
    reduced_counts.reserve(counts.size());
    for (size_t idx = 0; idx < counts.size(); ++idx) {
        if (counts[idx] <= 0)
            continue;
        reduced_ids.emplace_back(filtered_ids[idx]);
        reduced_counts.emplace_back(counts[idx]);
    }
    if (reduced_ids.empty())
        return {};
    filtered_ids = std::move(reduced_ids);
    counts = std::move(reduced_counts);

    const int total = std::accumulate(counts.begin(), counts.end(), 0);
    if (total <= 0)
        return std::vector<unsigned int>(filtered_ids.begin(), filtered_ids.end());

    const size_t cycle = size_t(total);

    std::vector<unsigned int> sequence;
    sequence.reserve(cycle);
    std::vector<int> emitted(counts.size(), 0);
    for (size_t pos = 0; pos < cycle; ++pos) {
        size_t best_idx = 0;
        double best_score = -1e9;
        for (size_t idx = 0; idx < counts.size(); ++idx) {
            const double target = double(pos + 1) * double(counts[idx]) / double(total);
            const double score = target - double(emitted[idx]);
            if (score > best_score) {
                best_score = score;
                best_idx = idx;
            }
        }
        ++emitted[best_idx];
        sequence.emplace_back(filtered_ids[best_idx]);
    }
    if (sequence.empty())
        sequence = filtered_ids;
    return sequence;
}

static std::string blend_display_color_from_sequence(const std::vector<std::string> &colors,
                                                     size_t                           num_physical,
                                                     const std::vector<unsigned int> &sequence,
                                                     const std::string               &fallback)
{
    if (colors.empty() || sequence.empty() || num_physical == 0)
        return fallback;

    std::vector<size_t> counts(num_physical + 1, size_t(0));
    size_t total = 0;
    for (const unsigned int id : sequence) {
        if (id == 0 || id > num_physical)
            continue;
        ++counts[id];
        ++total;
    }
    if (total == 0)
        return fallback;

    unsigned int first_id = 0;
    for (size_t id = 1; id <= num_physical; ++id) {
        if (counts[id] > 0) {
            first_id = unsigned(id);
            break;
        }
    }
    if (first_id == 0 || first_id > colors.size())
        return fallback;

    std::string blended = colors[first_id - 1];
    int         accumulated = int(counts[first_id]);
    for (size_t id = size_t(first_id + 1); id <= num_physical; ++id) {
        if (counts[id] == 0 || id > colors.size())
            continue;
        blended = MixedFilamentManager::blend_color(blended, colors[id - 1], accumulated, int(counts[id]));
        accumulated += int(counts[id]);
    }

    return blended;
}


std::string MixedFilamentConfigPanel::summarize_sequence(const std::vector<unsigned int> &seq)
{
    if (seq.empty()) return "";
    std::unordered_map<unsigned int, int> counts;
    for (unsigned int id : seq) counts[id]++;
    std::vector<std::pair<int, unsigned int>> sorted;
    for (auto &kv : counts) sorted.emplace_back(kv.second, kv.first);
    std::sort(sorted.begin(), sorted.end(), std::greater<>());
    std::string out;
    for (auto &p : sorted) {
        if (!out.empty()) out += "/";
        out += format_preview_sequence_percent(p.first, int(seq.size()));
    }
    return out;
}

std::string MixedFilamentConfigPanel::summarize_local_z_breakdown(const MixedFilament &mf,
                                                                 const std::vector<int> &weights,
                                                                 const MixedFilamentPreviewSettings &preview_settings)
{
    const std::string normalized_pattern = MixedFilamentManager::normalize_manual_pattern(mf.manual_pattern);
    if (!normalized_pattern.empty())
        return "Local-Z breakdown: manual pattern rows do not use pair decomposition.";

    if (mf.distribution_mode == int(MixedFilament::SameLayerPointillisme))
        return "Local-Z breakdown: same-layer mode does not use local-Z pair decomposition.";

    auto pair_name = [](unsigned int a, unsigned int b) {
        std::ostringstream ss;
        ss << 'F' << a << "+F" << b;
        return ss.str();
    };
    auto pair_split = [](unsigned int a, unsigned int b, int weight_a, int weight_b) {
        const int safe_a = std::max(0, weight_a);
        const int safe_b = std::max(0, weight_b);
        const int total  = std::max(1, safe_a + safe_b);
        const int pct_a  = int(std::lround(100.0 * double(safe_a) / double(total)));
        const int pct_b  = std::max(0, 100 - pct_a);

        std::ostringstream ss;
        ss << 'F' << a << "/F" << b << " " << safe_a << ':' << safe_b << " (" << pct_a << '/' << pct_b << ')';
        return ss.str();
    };
    auto cadence_entry = [&pair_name](unsigned int a, unsigned int b, int weight, int total) {
        const int pct = int(std::lround(100.0 * double(std::max(0, weight)) / double(std::max(1, total))));
        std::ostringstream ss;
        ss << pair_name(a, b) << ' ' << pct << '%';
        return ss.str();
    };

    const std::vector<unsigned int> ids = decode_gradient_ids(mf.gradient_component_ids);
    if (preview_settings.local_z_mode && preview_settings.local_z_direct_multicolor && ids.size() >= 3) {
        const std::vector<int> normalized = normalize_gradient_weights(weights, ids.size());
        const size_t effective_sublayers =
            mf.local_z_max_sublayers >= 2 ? size_t(std::max(2, mf.local_z_max_sublayers)) : ids.size();

        std::ostringstream ss;
        ss << "Local-Z direct multicolor solver: ";
        for (size_t idx = 0; idx < ids.size(); ++idx) {
            if (idx > 0)
                ss << ", ";
            const int pct = idx < normalized.size() ? normalized[idx] : 0;
            ss << 'F' << ids[idx] << ' ' << pct << '%';
        }
        ss << ".\nCarry-over error is distributed directly across all " << ids.size()
           << " components instead of collapsing them into pair cadence.";
        if (mf.local_z_max_sublayers >= 2)
            ss << "\nEffective Local-Z cap: up to " << effective_sublayers << " sublayers per nominal layer.";
        return ss.str();
    }

    if (ids.size() >= 4) {
        const std::vector<int> normalized = normalize_gradient_weights(weights, ids.size());
        const std::vector<unsigned int> pair_tokens = { 1, 2 };
        const std::vector<int> pair_weights = {
            std::max(1, normalized[0] + normalized[1]),
            std::max(1, normalized[2] + normalized[3])
        };
        const size_t max_pair_layers =
            (preview_settings.local_z_mode && mf.local_z_max_sublayers >= 2) ?
                std::max<size_t>(1, size_t(mf.local_z_max_sublayers) / 2) :
                size_t(0);
        const std::vector<unsigned int> uncapped_pair_sequence = build_weighted_multi_sequence(pair_tokens, pair_weights);
        const std::vector<unsigned int> effective_pair_sequence =
            max_pair_layers > 0 ? build_weighted_multi_sequence(pair_tokens, pair_weights, max_pair_layers) : uncapped_pair_sequence;
        const std::vector<unsigned int> &pair_sequence = effective_pair_sequence.empty() ? uncapped_pair_sequence : effective_pair_sequence;
        const int pair_ab_weight = int(std::count(pair_sequence.begin(), pair_sequence.end(), 1u));
        const int pair_cd_weight = int(std::count(pair_sequence.begin(), pair_sequence.end(), 2u));
        const int pair_total = std::max(1, int(pair_sequence.size()));

        std::ostringstream ss;
        ss << "Local-Z layer cadence: "
           << cadence_entry(ids[0], ids[1], pair_ab_weight, pair_total)
           << ", "
           << cadence_entry(ids[2], ids[3], pair_cd_weight, pair_total)
           << ".\nPair splits: "
           << pair_split(ids[0], ids[1], normalized[0], normalized[1])
           << ", "
           << pair_split(ids[2], ids[3], normalized[2], normalized[3])
           << '.';
        if (!preview_settings.local_z_mode && mf.local_z_max_sublayers >= 2)
            ss << "\nSaved row limit will apply when Local-Z dithering mode is enabled in print settings.";
        if (preview_settings.local_z_mode && mf.local_z_max_sublayers >= 2) {
            ss << "\nEffective Local-Z stack: " << (pair_total * 2) << " sublayers over " << pair_total << " pair layers";
            if (uncapped_pair_sequence.size() > pair_sequence.size())
                ss << " (uncapped " << (uncapped_pair_sequence.size() * 2) << ')';
            ss << '.';
        }
        return ss.str();
    }

    if (ids.size() == 3) {
        const std::vector<int> normalized = normalize_gradient_weights(weights, ids.size());
        const std::vector<unsigned int> pair_tokens = { 1, 2, 3 };
        const std::vector<int> pair_weights = {
            std::max(1, normalized[0] + normalized[1]),
            std::max(1, normalized[0] + normalized[2]),
            std::max(1, normalized[1] + normalized[2])
        };
        const size_t max_pair_layers =
            (preview_settings.local_z_mode && mf.local_z_max_sublayers >= 2) ?
                std::max<size_t>(1, size_t(mf.local_z_max_sublayers) / 2) :
                size_t(0);
        const std::vector<unsigned int> uncapped_pair_sequence = build_weighted_multi_sequence(pair_tokens, pair_weights);
        const std::vector<unsigned int> effective_pair_sequence =
            max_pair_layers > 0 ? build_weighted_multi_sequence(pair_tokens, pair_weights, max_pair_layers) : uncapped_pair_sequence;
        const std::vector<unsigned int> &pair_sequence = effective_pair_sequence.empty() ? uncapped_pair_sequence : effective_pair_sequence;
        const int pair_ab_weight = int(std::count(pair_sequence.begin(), pair_sequence.end(), 1u));
        const int pair_ac_weight = int(std::count(pair_sequence.begin(), pair_sequence.end(), 2u));
        const int pair_bc_weight = int(std::count(pair_sequence.begin(), pair_sequence.end(), 3u));
        const int pair_total     = std::max(1, int(pair_sequence.size()));

        std::ostringstream ss;
        ss << "Local-Z layer cadence: "
           << cadence_entry(ids[0], ids[1], pair_ab_weight, pair_total)
           << ", "
           << cadence_entry(ids[0], ids[2], pair_ac_weight, pair_total)
           << ", "
           << cadence_entry(ids[1], ids[2], pair_bc_weight, pair_total)
           << ".\nPair splits: "
           << pair_split(ids[0], ids[1], normalized[0], normalized[1])
           << ", "
           << pair_split(ids[0], ids[2], normalized[0], normalized[2])
           << ", "
           << pair_split(ids[1], ids[2], normalized[1], normalized[2])
           << '.';
        if (!preview_settings.local_z_mode && mf.local_z_max_sublayers >= 2)
            ss << "\nSaved row limit will apply when Local-Z dithering mode is enabled in print settings.";
        if (preview_settings.local_z_mode && mf.local_z_max_sublayers >= 2) {
            ss << "\nEffective Local-Z stack: " << (pair_total * 2) << " sublayers over " << pair_total << " pair layers";
            if (uncapped_pair_sequence.size() > pair_sequence.size())
                ss << " (uncapped " << (uncapped_pair_sequence.size() * 2) << ')';
            ss << '.';
        }
        return ss.str();
    }

    if (mf.component_a >= 1 && mf.component_b >= 1 && mf.component_a != mf.component_b) {
        const int pct_b = std::clamp(mf.mix_b_percent, 0, 100);
        const int pct_a = 100 - pct_b;
        std::ostringstream ss;
        ss << "Local-Z pair split: requested F" << mf.component_a << "/F" << mf.component_b
           << ' ' << pct_a << '/' << pct_b;
        if (preview_settings.local_z_mode) {
            const std::vector<double> effective_passes = build_local_z_preview_pass_heights(preview_settings.nominal_layer_height,
                                                                                            preview_settings.mixed_lower_bound,
                                                                                            preview_settings.mixed_upper_bound,
                                                                                            preview_settings.preferred_a_height,
                                                                                            preview_settings.preferred_b_height,
                                                                                            mf.mix_b_percent,
                                                                                            0);
            if (!effective_passes.empty()) {
                const int effective_pct_b = effective_local_z_preview_mix_b_percent(mf, preview_settings);
                ss << ", effective " << (100 - effective_pct_b) << '/' << effective_pct_b
                   << " over " << effective_passes.size() << " sublayers";
            }
        }
        ss << '.';
        return ss.str();
    }

    return "Local-Z breakdown: unavailable.";
}

std::string MixedFilamentConfigPanel::blend_from_sequence(const std::vector<std::string> &colors, const std::vector<unsigned int> &seq, const std::string &fallback)
{
    if (colors.empty() || seq.empty())
        return fallback;

    std::vector<size_t> counts(colors.size() + 1, size_t(0));
    size_t total = 0;
    for (const unsigned int id : seq) {
        if (id == 0 || id > colors.size())
            continue;
        ++counts[id];
        ++total;
    }
    if (total == 0)
        return fallback;

    unsigned int first_id = 0;
    for (size_t id = 1; id <= colors.size(); ++id) {
        if (counts[id] > 0) {
            first_id = unsigned(id);
            break;
        }
    }
    if (first_id == 0 || first_id > colors.size())
        return fallback;

    std::string blended = colors[first_id - 1];
    int acc = int(counts[first_id]);
    for (size_t id = size_t(first_id + 1); id <= colors.size(); ++id) {
        if (counts[id] == 0)
            continue;
        blended = MixedFilamentManager::blend_color(blended, colors[id - 1], acc, int(counts[id]));
        acc += int(counts[id]);
    }

    return blended;
}

MixedFilamentConfigPanel::MixedFilamentConfigPanel(wxWindow *parent,
                                                   size_t mixed_id,
                                                   const MixedFilament &mf,
                                                   size_t num_physical,
                                                   const std::vector<std::string> &physical_colors,
                                                   const std::vector<double> &nozzle_diameters,
                                                   const std::vector<wxColour> &palette,
                                                   const MixedFilamentPreviewSettings &preview_settings,
                                                   bool bias_mode_enabled,
                                                   OnChangeFn on_change)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_NONE)
    , m_mixed_id(mixed_id)
    , m_mf(mf)
    , m_num_physical(num_physical)
    , m_physical_colors(physical_colors)
    , m_nozzle_diameters(nozzle_diameters)
    , m_palette(palette)
    , m_preview_settings(preview_settings)
    , m_bias_mode_enabled(bias_mode_enabled)
    , m_selected_weight_state(std::make_shared<std::vector<int>>())
    , m_on_change(on_change)
{
    if (parent)
        SetBackgroundColour(parent->GetBackgroundColour());
    else
        SetBackgroundColour(wxGetApp().dark_mode() ? wxColour(52, 52, 56) : wxColour(255, 255, 255));
    build_ui();
}

void MixedFilamentConfigPanel::build_ui()
{
    const int gap = FromDIP(6);
    const int compact_gap = std::max(FromDIP(2), gap / 3);
    const bool is_dark = wxGetApp().dark_mode();
    const wxColour panel_bg = GetBackgroundColour().IsOk() ? GetBackgroundColour() :
        (is_dark ? wxColour(52, 52, 56) : wxColour(255, 255, 255));
    SetBackgroundColour(panel_bg);
    auto *root = new wxBoxSizer(wxVERTICAL);

    // Filament choices
    wxArrayString filament_choices;
    for (size_t i = 0; i < m_num_physical; ++i)
        filament_choices.Add(wxString::Format("F%d", int(i + 1)));
    wxArrayString optional_filament_choices;
    optional_filament_choices.Add(_L("None"));
    for (size_t i = 0; i < m_num_physical; ++i)
        optional_filament_choices.Add(wxString::Format("F%d", int(i + 1)));

    const int component_a = std::clamp(int(m_mf.component_a), 1, int(m_num_physical));
    const int component_b = std::clamp(int(m_mf.component_b), 1, int(m_num_physical));

    const std::vector<unsigned int> initial_gradient_ids = decode_gradient_ids(m_mf.gradient_component_ids);
    if (m_mf.distribution_mode == int(MixedFilament::SameLayerPointillisme)) {
        m_mf.distribution_mode = initial_gradient_ids.size() >= 3 ? int(MixedFilament::LayerCycle) : int(MixedFilament::Simple);
        m_mf.pointillism_all_filaments = false;
    }
    const int stored_distribution_mode = std::clamp(m_mf.distribution_mode,
                                                    int(MixedFilament::LayerCycle),
                                                    int(MixedFilament::Simple));
    const int row_distribution_mode = initial_gradient_ids.size() >= 3 ?
        (stored_distribution_mode == int(MixedFilament::Simple) ? int(MixedFilament::LayerCycle) : stored_distribution_mode) :
        int(MixedFilament::Simple);
    m_mf.distribution_mode = row_distribution_mode;
    const bool multi_gradient_row = row_distribution_mode != int(MixedFilament::Simple) && initial_gradient_ids.size() >= 3;
    const int selection_c = initial_gradient_ids.size() >= 3 ? int(initial_gradient_ids[2]) : 0;
    const int selection_d = initial_gradient_ids.size() >= 4 ? int(initial_gradient_ids[3]) : 0;

    // Hidden data controls used as backing state for swatch pickers.
    m_choice_a = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, filament_choices);
    m_choice_b = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, filament_choices);
    m_choice_a->SetSelection(component_a - 1);
    m_choice_b->SetSelection(component_b - 1);
    m_choice_a->Hide();
    m_choice_b->Hide();
    if (multi_gradient_row) {
        m_choice_c = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, optional_filament_choices);
        m_choice_c->SetSelection(std::clamp(selection_c, 0, int(m_num_physical)));
        m_choice_c->Hide();
        if (initial_gradient_ids.size() >= 4) {
            m_choice_d = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, optional_filament_choices);
            m_choice_d->SetSelection(std::clamp(selection_d, 0, int(m_num_physical)));
            m_choice_d->Hide();
        }
    }

    auto create_component_picker = [this, gap](wxPanel *&container_out, wxPanel *&swatch_out, wxStaticText *&label_out, const wxString &tooltip) {
        const int inner_gap = std::max(FromDIP(1), gap / 4);
        const bool local_is_dark = wxGetApp().dark_mode();
        const wxColour local_picker_bg = local_is_dark ? wxColour(64, 64, 70) : wxColour(255, 255, 255);
        const wxColour local_picker_text = local_is_dark ? wxColour(230, 230, 230) : wxColour(32, 32, 32);
        container_out = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
        container_out->SetBackgroundColour(local_picker_bg);
        const wxSize picker_size(FromDIP(38), FromDIP(22));
        container_out->SetMinSize(picker_size);
        container_out->SetMaxSize(picker_size);

        auto *container_sizer = new wxBoxSizer(wxHORIZONTAL);
        swatch_out = new wxPanel(container_out, wxID_ANY, wxDefaultPosition, wxSize(FromDIP(12), FromDIP(12)), wxBORDER_SIMPLE);
        swatch_out->SetMinSize(wxSize(FromDIP(12), FromDIP(12)));
        swatch_out->SetToolTip(tooltip);
        label_out = new wxStaticText(container_out, wxID_ANY, wxEmptyString);
        label_out->SetForegroundColour(local_picker_text);
        label_out->SetToolTip(tooltip);

        auto *content_sizer = new wxBoxSizer(wxHORIZONTAL);
        content_sizer->Add(swatch_out, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, inner_gap);
        content_sizer->Add(label_out, 0, wxALIGN_CENTER_VERTICAL);
        container_sizer->AddStretchSpacer(1);
        container_sizer->Add(content_sizer, 0, wxALIGN_CENTER_VERTICAL);
        container_sizer->AddStretchSpacer(1);
        container_out->SetSizer(container_sizer);
        container_out->SetToolTip(tooltip);
        container_out->SetCursor(wxCursor(wxCURSOR_HAND));
        swatch_out->SetCursor(wxCursor(wxCURSOR_HAND));
        label_out->SetCursor(wxCursor(wxCURSOR_HAND));
    };

    create_component_picker(m_picker_a_container, m_picker_a_swatch, m_picker_a_label, _L("Click to choose a physical filament color"));
    create_component_picker(m_picker_b_container, m_picker_b_swatch, m_picker_b_label, _L("Click to choose a physical filament color"));
    if (m_choice_c)
        create_component_picker(m_picker_c_container, m_picker_c_swatch, m_picker_c_label, _L("Click to choose a physical filament color"));
    if (m_choice_d)
        create_component_picker(m_picker_d_container, m_picker_d_swatch, m_picker_d_label, _L("Click to choose a physical filament color"));
    update_component_picker_visuals();

    // Check for pattern mode
    const std::string normalized_pattern = MixedFilamentManager::normalize_manual_pattern(m_mf.manual_pattern);
    const bool pattern_row_mode = !normalized_pattern.empty();

    auto *picker_row = new wxBoxSizer(wxHORIZONTAL);
    if (!pattern_row_mode) {
        auto add_picker = [this, picker_row, gap](wxPanel *container, bool &first_picker) {
            if (!container)
                return;
            if (!first_picker)
                picker_row->Add(new wxStaticText(this, wxID_ANY, "+"), 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, std::max(FromDIP(2), gap / 2));
            picker_row->Add(container, 0, wxALIGN_CENTER_VERTICAL);
            first_picker = false;
        };

        bool first_picker = true;
        add_picker(m_picker_a_container, first_picker);
        add_picker(m_picker_b_container, first_picker);
        add_picker(m_picker_c_container, first_picker);
        add_picker(m_picker_d_container, first_picker);
    } else {
        if (m_picker_a_container) m_picker_a_container->Hide();
        if (m_picker_b_container) m_picker_b_container->Hide();
        if (m_picker_c_container) m_picker_c_container->Hide();
        if (m_picker_d_container) m_picker_d_container->Hide();
    }
    root->Add(picker_row, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, gap);

    // Pattern controls (if pattern mode)
    if (pattern_row_mode) {
        auto *pattern_row = new wxBoxSizer(wxHORIZONTAL);
        auto *pattern_label = new wxStaticText(this, wxID_ANY, _L("Pattern"));
        pattern_label->SetForegroundColour(is_dark ? wxColour(236, 236, 236) : wxColour(20, 20, 20));
        pattern_row->Add(pattern_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, gap);
        m_pattern_ctrl = new wxTextCtrl(this, wxID_ANY, from_u8(normalized_pattern), wxDefaultPosition,
                                        wxSize(FromDIP(200), -1), wxTE_PROCESS_ENTER);
        m_pattern_ctrl->SetToolTip(_L("Manual repeating pattern. Digits 1-9 for filament IDs 1-9. "
                                      "Use [N] for IDs >= 10 (e.g. [12]). "
                                      "Use commas to define per-perimeter groups, e.g. 12,21. "
                                      "Example: 11112222, 12,21, or 1234."));
        pattern_row->Add(m_pattern_ctrl, 1, wxALIGN_CENTER_VERTICAL);
        root->Add(pattern_row, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, gap);

        auto *quick_buttons = new wxBoxSizer(wxHORIZONTAL);
        for (size_t fid = 0; fid < m_num_physical; ++fid) {
            wxButton *btn = new wxButton(this, wxID_ANY, wxString::Format("%d", int(fid + 1)),
                                         wxDefaultPosition, wxSize(FromDIP(24), FromDIP(22)), wxBU_EXACTFIT);
            const wxColour chip_color = (fid < m_palette.size()) ? m_palette[fid] : wxColour("#26A69A");
            btn->SetBackgroundColour(chip_color);
            btn->SetToolTip(wxString::Format(_L("Append filament %d to pattern"), int(fid + 1)));
            quick_buttons->Add(btn, 0, wxRIGHT, FromDIP(4));
            m_pattern_quick_buttons.emplace_back(btn);
        }
        auto *filaments_label = new wxStaticText(this, wxID_ANY, _L("Filaments"));
        filaments_label->SetForegroundColour(is_dark ? wxColour(236, 236, 236) : wxColour(20, 20, 20));
        picker_row->Add(filaments_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, std::max(FromDIP(3), gap / 2));
        picker_row->Add(quick_buttons, 0, wxALIGN_CENTER_VERTICAL);
    } else {
        // Blend selector for non-pattern mode
        const bool simple_mode = row_distribution_mode == int(MixedFilament::Simple);
        std::vector<unsigned int> selected_gradient_ids = simple_mode ? std::vector<unsigned int>() : initial_gradient_ids;
        if (selected_gradient_ids.size() < 3) selected_gradient_ids.clear();
        if (selected_gradient_ids.empty()) {
            selected_gradient_ids.emplace_back(unsigned(component_a));
            if (component_b != component_a) selected_gradient_ids.emplace_back(unsigned(component_b));
        }
        const bool multi_gradient_mode = selected_gradient_ids.size() >= 3;
        *m_selected_weight_state = normalize_gradient_weights(
            decode_gradient_weights(m_mf.gradient_component_weights, selected_gradient_ids.size()),
            selected_gradient_ids.size());

        wxColour color_a = (component_a >= 1 && component_a <= int(m_palette.size())) ? m_palette[component_a - 1] : wxColour("#26A69A");
        wxColour color_b = (component_b >= 1 && component_b <= int(m_palette.size())) ? m_palette[component_b - 1] : wxColour("#26A69A");
        m_blend_selector = new MixedGradientSelector(this, color_a, color_b, std::clamp(m_mf.mix_b_percent, 0, 100));
        m_blend_selector->SetBackgroundColour(panel_bg);
        m_blend_label = nullptr;
        picker_row->AddSpacer(gap);
        picker_row->Add(m_blend_selector, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxLEFT, gap);

        if (m_blend_selector) {
            std::vector<wxColour> corner_colors;
            corner_colors.reserve(selected_gradient_ids.size());
            for (const unsigned int id : selected_gradient_ids) {
                if (id >= 1 && id <= m_palette.size())
                    corner_colors.emplace_back(m_palette[id - 1]);
            }
            if (!simple_mode && corner_colors.size() >= 3)
                m_blend_selector->set_multi_preview(corner_colors, *m_selected_weight_state);
        }
    }

    // Preview
    auto *preview_row = new wxBoxSizer(wxHORIZONTAL);
    m_mix_preview = new MixedMixPreview(this);
    m_mix_preview->SetBackgroundColour(panel_bg);
    preview_row->Add(m_mix_preview, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxRIGHT, compact_gap);

    auto *bias_controls = new wxBoxSizer(wxHORIZONTAL);
    const float initial_surface_offset_value = mixed_filament_single_surface_offset_value(m_mf, m_nozzle_diameters);
    const double initial_bias_limit = mixed_filament_bias_limit_mm(m_mf, m_nozzle_diameters);
    const wxString bias_tooltip =
        _L("Positive bias recesses the second filament in the pair; negative bias recesses the first filament.\n\n"
           "The color chip shows which filament the current value affects.\n\n"
           "Grouped wall patterns and Local-Z dithering ignore it.");

    auto *surface_offset_label = new wxStaticText(this, wxID_ANY, _L("Bias"));
    surface_offset_label->SetForegroundColour(is_dark ? wxColour(236, 236, 236) : wxColour(20, 20, 20));
    surface_offset_label->SetToolTip(bias_tooltip);
    bias_controls->Add(surface_offset_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, compact_gap);

    create_component_picker(m_surface_offset_target_container,
                            m_surface_offset_target_swatch,
                            m_surface_offset_target_label,
                            bias_tooltip);
    if (m_surface_offset_target_container)
        m_surface_offset_target_container->SetCursor(wxCursor(wxCURSOR_ARROW));
    if (m_surface_offset_target_swatch)
        m_surface_offset_target_swatch->SetCursor(wxCursor(wxCURSOR_ARROW));
    if (m_surface_offset_target_label)
        m_surface_offset_target_label->SetCursor(wxCursor(wxCURSOR_ARROW));
    bias_controls->Add(m_surface_offset_target_container, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, compact_gap);

    m_surface_offset_spin = new wxSpinCtrlDouble(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(FromDIP(58), -1),
                                                 wxSP_ARROW_KEYS | wxALIGN_RIGHT | wxTE_PROCESS_ENTER,
                                                 -initial_bias_limit, initial_bias_limit,
                                                 std::clamp(double(initial_surface_offset_value), -initial_bias_limit, initial_bias_limit), 0.001);
    m_surface_offset_spin->SetDigits(3);
    m_surface_offset_spin->SetToolTip(bias_tooltip);
    bias_controls->Add(m_surface_offset_spin, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, compact_gap);

    auto *surface_offset_units = new wxStaticText(this, wxID_ANY, _L("mm"));
    surface_offset_units->SetForegroundColour(is_dark ? wxColour(210, 210, 210) : wxColour(72, 72, 72));
    surface_offset_units->SetToolTip(bias_tooltip);
    bias_controls->Add(surface_offset_units, 0, wxALIGN_CENTER_VERTICAL);
    if (m_bias_mode_enabled)
        preview_row->Add(bias_controls, 0, wxALIGN_CENTER_VERTICAL);
    else {
        surface_offset_label->Hide();
        if (m_surface_offset_target_container)
            m_surface_offset_target_container->Hide();
        if (m_surface_offset_spin)
            m_surface_offset_spin->Hide();
        surface_offset_units->Hide();
    }
    root->Add(preview_row, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, gap);

    if (m_bias_mode_enabled) {
        const auto initial_surface_offset_pair =
            mixed_filament_single_surface_offset_pair(m_mf, initial_surface_offset_value, m_nozzle_diameters);
        m_mf.component_a_surface_offset = initial_surface_offset_pair.first;
        m_mf.component_b_surface_offset = initial_surface_offset_pair.second;
    }

    const bool initial_component_surface_offsets_supported = m_bias_mode_enabled &&
                                                             !pattern_row_mode &&
                                                             row_distribution_mode != int(MixedFilament::SameLayerPointillisme) &&
                                                             !m_preview_settings.local_z_mode;
    if (m_surface_offset_spin)
        m_surface_offset_spin->Enable(initial_component_surface_offsets_supported);

    const bool local_z_limit_supported = multi_gradient_row &&
                                         row_distribution_mode != int(MixedFilament::SameLayerPointillisme);
    if (local_z_limit_supported) {
        auto *local_z_limit_row = new wxBoxSizer(wxHORIZONTAL);
        m_local_z_limit_checkbox = new wxCheckBox(this, wxID_ANY, _L("Limit Local-Z"));
        m_local_z_limit_checkbox->SetValue(m_mf.local_z_max_sublayers >= 2);
        m_local_z_limit_checkbox->SetForegroundColour(is_dark ? wxColour(236, 236, 236) : wxColour(20, 20, 20));
        m_local_z_limit_checkbox->SetToolTip(
            _L("Store a per-color Local-Z cadence cap. It applies when Local-Z dithering mode is enabled in print settings."));
        local_z_limit_row->Add(m_local_z_limit_checkbox, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, gap);

        auto *local_z_limit_label = new wxStaticText(this, wxID_ANY, _L("Max sublayers"));
        local_z_limit_label->SetForegroundColour(is_dark ? wxColour(236, 236, 236) : wxColour(20, 20, 20));
        local_z_limit_row->Add(local_z_limit_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, std::max(FromDIP(3), gap / 2));

        const int initial_local_z_limit = std::max(2, m_mf.local_z_max_sublayers > 0 ? m_mf.local_z_max_sublayers : 6);
        m_local_z_limit_spin = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(FromDIP(72), -1),
                                              wxSP_ARROW_KEYS | wxALIGN_RIGHT | wxTE_PROCESS_ENTER, 2, 999, initial_local_z_limit);
        m_local_z_limit_spin->SetToolTip(
            _L("Maximum number of Local-Z sublayers this color may use before its cadence repeats."));
        local_z_limit_row->Add(m_local_z_limit_spin, 0, wxALIGN_CENTER_VERTICAL);

        const bool enable_local_z_limit_controls = m_local_z_limit_checkbox->GetValue();
        m_local_z_limit_spin->Enable(enable_local_z_limit_controls);
        root->Add(local_z_limit_row, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, gap);
    }

    m_breakdown_label = new wxStaticText(this, wxID_ANY, wxEmptyString);
    m_breakdown_label->SetForegroundColour(is_dark ? wxColour(210, 210, 210) : wxColour(72, 72, 72));
    m_breakdown_label->Wrap(FromDIP(360));
    root->Add(m_breakdown_label, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, gap);

    // Bind events
    auto apply_changes = [this]() {
        m_has_changes = true;

        double surface_offset_value = 0.0;
        if (m_surface_offset_spin) {
            surface_offset_value = m_surface_offset_spin->GetValue();
#if !defined(wxHAS_NATIVE_SPINCTRLDOUBLE)
            if (wxTextCtrl *text = m_surface_offset_spin->GetText()) {
                double parsed_value = 0.0;
                if (text->GetValue().ToDouble(&parsed_value))
                    surface_offset_value = parsed_value;
            }
#endif
        }

        int a = std::clamp(m_choice_a->GetSelection() + 1, 1, int(m_num_physical));
        int b = std::clamp(m_choice_b->GetSelection() + 1, 1, int(m_num_physical));
        if (a == b && m_num_physical > 1) {
            b = (a == int(m_num_physical)) ? 1 : a + 1;
            m_choice_b->SetSelection(b - 1);
        }
        update_component_picker_visuals();

        if (m_local_z_limit_spin)
            m_local_z_limit_spin->Enable(m_local_z_limit_checkbox != nullptr &&
                                         m_local_z_limit_checkbox->GetValue());

        m_mf.component_a = unsigned(a);
        m_mf.component_b = unsigned(b);
        if (m_bias_mode_enabled) {
            const double bias_limit = mixed_filament_bias_limit_mm(m_mf, m_nozzle_diameters);
            const float clamped_surface_offset_value = std::clamp(float(surface_offset_value), -float(bias_limit), float(bias_limit));
            const auto surface_offset_pair =
                mixed_filament_single_surface_offset_pair(m_mf, clamped_surface_offset_value, m_nozzle_diameters);
            m_mf.component_a_surface_offset = surface_offset_pair.first;
            m_mf.component_b_surface_offset = surface_offset_pair.second;
            if (m_surface_offset_spin)
                m_surface_offset_spin->SetValue(clamped_surface_offset_value);
        }
        m_mf.local_z_max_sublayers =
            (m_local_z_limit_checkbox != nullptr && m_local_z_limit_checkbox->GetValue() && m_local_z_limit_spin != nullptr) ?
                std::max(2, m_local_z_limit_spin->GetValue()) :
                0;

        bool simple_mode = true;
        bool same_layer_mode = false;
        int preview_mix_b_percent = std::clamp(m_mf.mix_b_percent, 0, 100);
        std::vector<unsigned int> preview_sequence;

        if (m_pattern_ctrl) {
            m_mf.distribution_mode = int(MixedFilament::Simple);
            std::string normalized = MixedFilamentManager::normalize_manual_pattern(into_u8(m_pattern_ctrl->GetValue()));
            if (normalized.empty()) normalized = "12";
            if (into_u8(m_pattern_ctrl->GetValue()) != normalized)
                m_pattern_ctrl->ChangeValue(from_u8(normalized));
            m_mf.manual_pattern = normalized;
            m_mf.mix_b_percent = MixedFilamentManager::mix_percent_from_manual_pattern(normalized);
            m_mf.pointillism_all_filaments = false;
            m_mf.gradient_component_ids.clear();
            m_mf.gradient_component_weights.clear();
            preview_sequence = decode_manual_pattern_ids(m_mf.manual_pattern,
                                                         m_mf.component_a,
                                                         m_mf.component_b,
                                                         m_num_physical,
                                                         m_preview_settings.wall_loops);
        } else {
            std::vector<unsigned int> selected_ids;
            selected_ids.reserve(4);
            auto add_unique = [&selected_ids](unsigned int id) {
                if (id == 0) return;
                if (std::find(selected_ids.begin(), selected_ids.end(), id) == selected_ids.end())
                    selected_ids.emplace_back(id);
            };
            add_unique(unsigned(a));
            add_unique(unsigned(b));
            if (m_choice_c && m_choice_c->GetSelection() > 0)
                add_unique(unsigned(m_choice_c->GetSelection()));
            if (m_choice_d && m_choice_d->GetSelection() > 0)
                add_unique(unsigned(m_choice_d->GetSelection()));
            const bool multi_gradient_mode = selected_ids.size() >= 3;
            m_mf.distribution_mode = multi_gradient_mode ? int(MixedFilament::LayerCycle) : int(MixedFilament::Simple);
            simple_mode = m_mf.distribution_mode == int(MixedFilament::Simple);
            m_mf.mix_b_percent = std::clamp(m_blend_selector ? m_blend_selector->value() : 50, 0, 100);
            m_mf.manual_pattern.clear();
            m_mf.pointillism_all_filaments = false;

            const wxColour color_a = (a >= 1 && a <= int(m_palette.size())) ? m_palette[size_t(a - 1)] : wxColour("#26A69A");
            const wxColour color_b = (b >= 1 && b <= int(m_palette.size())) ? m_palette[size_t(b - 1)] : wxColour("#26A69A");
            if (m_blend_selector) {
                if (!simple_mode && multi_gradient_mode) {
                    std::vector<wxColour> corner_colors;
                    corner_colors.reserve(selected_ids.size());
                    for (const unsigned int id : selected_ids) {
                        if (id >= 1 && id <= m_palette.size())
                            corner_colors.emplace_back(m_palette[id - 1]);
                    }
                    if (corner_colors.size() >= 3)
                        m_blend_selector->set_multi_preview(corner_colors, *m_selected_weight_state);
                    else
                        m_blend_selector->set_colors(color_a, color_b);
                } else {
                    m_blend_selector->set_colors(color_a, color_b);
                }
            }

            if (multi_gradient_mode) {
                const std::vector<int> decoded_weights =
                    decode_gradient_weights(m_mf.gradient_component_weights, selected_ids.size());
                if (m_selected_weight_state->size() != selected_ids.size())
                    *m_selected_weight_state = decoded_weights;
                *m_selected_weight_state = normalize_gradient_weights(*m_selected_weight_state, selected_ids.size());
                m_mf.gradient_component_ids = encode_gradient_ids(selected_ids);
                m_mf.gradient_component_weights = encode_gradient_weights(*m_selected_weight_state);
                preview_sequence = build_weighted_multi_sequence(selected_ids, *m_selected_weight_state);
            } else {
                m_mf.gradient_component_ids.clear();
                m_mf.gradient_component_weights.clear();
                preview_mix_b_percent = effective_local_z_preview_mix_b_percent(m_mf, m_preview_settings);
                preview_sequence = build_weighted_pair_sequence(m_mf.component_a, m_mf.component_b, preview_mix_b_percent, same_layer_mode);
            }
        }
        m_mf.custom = true;

        const std::vector<unsigned int> selected_gradient_ids = decode_gradient_ids(m_mf.gradient_component_ids);
        const bool component_surface_offsets_supported = m_bias_mode_enabled &&
                                                         (m_pattern_ctrl == nullptr) &&
                                                         !same_layer_mode &&
                                                         !m_preview_settings.local_z_mode;
        if (m_surface_offset_spin)
            m_surface_offset_spin->Enable(component_surface_offsets_supported);
        if (preview_sequence.empty())
            preview_sequence = build_weighted_pair_sequence(m_mf.component_a, m_mf.component_b, preview_mix_b_percent, same_layer_mode);

        if (m_blend_selector && selected_gradient_ids.size() >= 3) {
            std::vector<wxColour> corner_colors;
            corner_colors.reserve(selected_gradient_ids.size());
            for (const unsigned int id : selected_gradient_ids) {
                if (id >= 1 && id <= m_palette.size())
                    corner_colors.emplace_back(m_palette[id - 1]);
            }
            if (corner_colors.size() >= 3)
                m_blend_selector->set_multi_preview(corner_colors, *m_selected_weight_state);
        }

        if (Slic3r::mixed_filament_supports_bias_apparent_color(m_mf, m_preview_settings, m_bias_mode_enabled) &&
            m_mf.component_a >= 1 && m_mf.component_b >= 1 &&
            m_mf.component_a <= m_physical_colors.size() && m_mf.component_b <= m_physical_colors.size()) {
            const auto [apparent_pct_a, apparent_pct_b] =
                Slic3r::mixed_filament_apparent_pair_percentages(m_mf, m_preview_settings, m_nozzle_diameters, m_bias_mode_enabled);
            m_mf.display_color = MixedFilamentManager::blend_color(
                m_physical_colors[size_t(m_mf.component_a - 1)],
                m_physical_colors[size_t(m_mf.component_b - 1)],
                apparent_pct_a,
                apparent_pct_b);
        } else if (selected_gradient_ids.size() >= 3 || !preview_sequence.empty()) {
            m_mf.display_color = blend_from_sequence(m_physical_colors, preview_sequence, "#26A69A");
            if (m_blend_label) {
                if (selected_gradient_ids.size() >= 3) {
                    m_blend_label->SetLabel(wxString::Format(_L("%d-color layer cycle"), int(selected_gradient_ids.size())));
                } else {
                    m_blend_label->SetLabel(wxString::Format(simple_mode ? _L("Simple %d%%/%d%%") : _L("%d%%/%d%%"),
                                                            100 - preview_mix_b_percent, preview_mix_b_percent));
                }
            }
        } else {
            m_mf.display_color = MixedFilamentManager::blend_color(
                m_physical_colors[size_t(a - 1)], m_physical_colors[size_t(b - 1)],
                100 - preview_mix_b_percent, preview_mix_b_percent);
            if (m_blend_label)
                m_blend_label->SetLabel(wxString::Format(simple_mode ? _L("Simple %d%%/%d%%") : _L("%d%%/%d%%"),
                                                        100 - preview_mix_b_percent, preview_mix_b_percent));
        }

        if (m_mix_preview) {
            const std::string bias_summary =
                mixed_filament_apparent_pair_summary(m_mf, m_preview_settings, m_nozzle_diameters, m_bias_mode_enabled);
            const std::string summary = bias_summary.empty() ? summarize_sequence(preview_sequence) : bias_summary;
            std::vector<double> preview_surface_offsets(m_palette.size() + 1, 0.0);
            if (m_bias_mode_enabled && m_mf.component_a >= 1 && m_mf.component_a < preview_surface_offsets.size())
                preview_surface_offsets[m_mf.component_a] = double(m_mf.component_a_surface_offset);
            if (m_bias_mode_enabled && m_mf.component_b >= 1 && m_mf.component_b < preview_surface_offsets.size())
                preview_surface_offsets[m_mf.component_b] = double(m_mf.component_b_surface_offset);
            m_mix_preview->set_data(m_palette, preview_sequence, same_layer_mode, preview_surface_offsets, wxColour(m_mf.display_color),
                                    _L("Preview"), summary.empty() ? wxString() : from_u8(summary));
        }
        update_local_z_breakdown();
        if (m_swatch) {
            m_swatch->SetBackgroundColour(wxColour(m_mf.display_color));
            m_swatch->Refresh();
        }
        if (m_on_change)
            m_on_change(m_mf);
    };

    auto make_color_chip_bitmap = [this](const wxColour &color) {
        const int chip_size = FromDIP(14);
        wxBitmap bmp(chip_size, chip_size);
        wxMemoryDC dc(bmp);
        dc.SetBackground(wxBrush(wxColour(255, 255, 255)));
        dc.Clear();
        dc.SetPen(wxPen(wxColour(120, 120, 120)));
        dc.SetBrush(wxBrush(color));
        dc.DrawRectangle(0, 0, chip_size, chip_size);
        dc.SelectObject(wxNullBitmap);
        return bmp;
    };

    auto bind_component_picker_popup = [this, apply_changes, make_color_chip_bitmap](wxWindow *target, wxChoice *backing_choice) {
        if (!target || !backing_choice)
            return;

        target->Bind(wxEVT_LEFT_UP, [this, apply_changes, make_color_chip_bitmap, backing_choice](wxMouseEvent &) {
            if (m_num_physical == 0)
                return;

            const bool allow_none = backing_choice->GetCount() == unsigned(m_num_physical + 1);
            wxMenu menu;
            std::vector<int> item_ids;
            item_ids.reserve(m_num_physical + (allow_none ? 1 : 0));
            if (allow_none) {
                const int item_id = wxWindow::NewControlId();
                item_ids.emplace_back(item_id);
                menu.Append(item_id, backing_choice->GetSelection() == 0 ? _L("None (Selected)") : _L("None"));
            }
            for (size_t i = 0; i < m_num_physical; ++i) {
                const int item_id = wxWindow::NewControlId();
                item_ids.emplace_back(item_id);
                const int selection_index = allow_none ? int(i + 1) : int(i);
                const bool is_selected = selection_index == backing_choice->GetSelection();
                const wxString item_label = wxString::Format("F%d%s", int(i + 1), is_selected ? " (Selected)" : "");
                auto *menu_item = new wxMenuItem(&menu, item_id, item_label, wxEmptyString, wxITEM_NORMAL);
                const wxColour item_color = (i < m_palette.size()) ? m_palette[i] : wxColour("#26A69A");
                menu_item->SetBitmap(make_color_chip_bitmap(item_color));
                menu.Append(menu_item);
            }

            menu.Bind(wxEVT_COMMAND_MENU_SELECTED, [apply_changes, backing_choice, item_ids](wxCommandEvent &evt) {
                const auto it = std::find(item_ids.begin(), item_ids.end(), evt.GetId());
                if (it == item_ids.end())
                    return;
                const int selection = int(std::distance(item_ids.begin(), it));
                backing_choice->SetSelection(selection);
                apply_changes();
            });
            PopupMenu(&menu);
        });
    };

    bind_component_picker_popup(m_picker_a_container, m_choice_a);
    bind_component_picker_popup(m_picker_a_swatch, m_choice_a);
    bind_component_picker_popup(m_picker_a_label, m_choice_a);
    bind_component_picker_popup(m_picker_b_container, m_choice_b);
    bind_component_picker_popup(m_picker_b_swatch, m_choice_b);
    bind_component_picker_popup(m_picker_b_label, m_choice_b);
    bind_component_picker_popup(m_picker_c_container, m_choice_c);
    bind_component_picker_popup(m_picker_c_swatch, m_choice_c);
    bind_component_picker_popup(m_picker_c_label, m_choice_c);
    bind_component_picker_popup(m_picker_d_container, m_choice_d);
    bind_component_picker_popup(m_picker_d_swatch, m_choice_d);
    bind_component_picker_popup(m_picker_d_label, m_choice_d);

    m_choice_a->Bind(wxEVT_CHOICE, [apply_changes](wxCommandEvent&) { apply_changes(); });
    m_choice_b->Bind(wxEVT_CHOICE, [apply_changes](wxCommandEvent&) { apply_changes(); });
    if (m_choice_c)
        m_choice_c->Bind(wxEVT_CHOICE, [apply_changes](wxCommandEvent&) { apply_changes(); });
    if (m_choice_d)
        m_choice_d->Bind(wxEVT_CHOICE, [apply_changes](wxCommandEvent&) { apply_changes(); });
    if (m_blend_selector)
        m_blend_selector->Bind(wxEVT_SLIDER, [apply_changes](wxCommandEvent&) { apply_changes(); });
    if (m_local_z_limit_checkbox)
        m_local_z_limit_checkbox->Bind(wxEVT_CHECKBOX, [apply_changes](wxCommandEvent &) { apply_changes(); });
    if (m_local_z_limit_spin) {
        m_local_z_limit_spin->Bind(wxEVT_SPINCTRL, [apply_changes](wxCommandEvent &) { apply_changes(); });
        m_local_z_limit_spin->Bind(wxEVT_TEXT_ENTER, [apply_changes](wxCommandEvent &) { apply_changes(); });
        m_local_z_limit_spin->Bind(wxEVT_KILL_FOCUS, [apply_changes](wxFocusEvent &evt) {
            apply_changes();
            evt.Skip();
        });
    }
    if (m_surface_offset_spin) {
        m_surface_offset_spin->Bind(wxEVT_SPINCTRLDOUBLE, [apply_changes](wxSpinDoubleEvent &) { apply_changes(); });
        m_surface_offset_spin->Bind(wxEVT_TEXT_ENTER, [apply_changes](wxCommandEvent &) { apply_changes(); });
        m_surface_offset_spin->Bind(wxEVT_KILL_FOCUS, [apply_changes](wxFocusEvent &evt) {
            apply_changes();
            evt.Skip();
        });
    }

    if (m_blend_selector) {
        m_blend_selector->Bind(wxEVT_BUTTON, [this, apply_changes](wxCommandEvent&) {
            if (!m_blend_selector->is_multi_mode()) return;
            std::vector<unsigned int> selected_ids;
            auto add_unique = [&selected_ids](unsigned int id) { if (id > 0 && std::find(selected_ids.begin(), selected_ids.end(), id) == selected_ids.end()) selected_ids.emplace_back(id); };
            add_unique(unsigned(std::clamp(m_choice_a ? (m_choice_a->GetSelection() + 1) : 0, 1, int(m_num_physical))));
            add_unique(unsigned(std::clamp(m_choice_b ? (m_choice_b->GetSelection() + 1) : 0, 1, int(m_num_physical))));
            if (m_choice_c && m_choice_c->GetSelection() > 0) add_unique(unsigned(m_choice_c->GetSelection()));
            if (m_choice_d && m_choice_d->GetSelection() > 0) add_unique(unsigned(m_choice_d->GetSelection()));
            if (selected_ids.size() < 3) return;
            const std::vector<int> initial_weights = normalize_gradient_weights(*m_selected_weight_state, selected_ids.size());
            MixedGradientWeightsDialog dlg(this, selected_ids, m_palette, initial_weights);
            if (dlg.ShowModal() != wxID_OK) return;
            *m_selected_weight_state = dlg.normalized_weights();
            apply_changes();
        });
    }

    if (m_pattern_ctrl) {
        auto append_pattern_token = [this](int filament_id) {
            if (!m_pattern_ctrl || filament_id <= 0) return;
            if (filament_id >= 10)
                m_pattern_ctrl->AppendText(wxString::Format("[%d]", filament_id));
            else
                m_pattern_ctrl->AppendText(wxString::Format("%d", filament_id));
        };
        m_pattern_ctrl->Bind(wxEVT_TEXT_ENTER, [apply_changes](wxCommandEvent&) { apply_changes(); });
        m_pattern_ctrl->Bind(wxEVT_KILL_FOCUS, [apply_changes](wxFocusEvent &evt) { apply_changes(); evt.Skip(); });
        for (size_t fid = 0; fid < m_pattern_quick_buttons.size(); ++fid) {
            wxButton *btn = m_pattern_quick_buttons[fid];
            if (btn) {
                const int filament_id = int(fid + 1);
                btn->Bind(wxEVT_BUTTON, [apply_changes, append_pattern_token, filament_id](wxCommandEvent&) {
                    append_pattern_token(filament_id);
                    apply_changes();
                });
            }
        }
    }

    update_component_picker_visuals();
    SetSizer(root);
    Layout();
    SetMinSize(wxSize(-1, GetBestSize().GetHeight()));
    update_preview();
}

void MixedFilamentConfigPanel::update_component_picker_visuals()
{
    auto update_one = [this](wxChoice *choice, wxPanel *container, wxPanel *swatch, wxStaticText *label) {
        if (!choice)
            return;
        int sel = choice->GetSelection();
        const bool allow_none = choice->GetCount() == unsigned(m_num_physical + 1);
        if (sel < 0 && m_num_physical > 0) {
            sel = 0;
            choice->SetSelection(sel);
        }
        if (sel < 0)
            return;

        if (allow_none && sel == 0) {
            const wxColour none_color = wxGetApp().dark_mode() ? wxColour(86, 86, 92) : wxColour(224, 224, 224);
            if (swatch) {
                swatch->SetBackgroundColour(none_color);
                swatch->Refresh();
            }
            if (label)
                label->SetLabel(_L("None"));
            if (container) {
                container->Layout();
                container->Refresh();
            }
            return;
        }

        const int color_idx = allow_none ? sel - 1 : sel;
        const wxColour color = (color_idx >= 0 && size_t(color_idx) < m_palette.size()) ? m_palette[size_t(color_idx)] : wxColour("#26A69A");
        if (swatch) {
            swatch->SetBackgroundColour(color);
            swatch->Refresh();
        }
        if (label)
            label->SetLabel(wxString::Format("F%d", color_idx + 1));
        if (container) {
            container->Layout();
            container->Refresh();
        }
    };

    update_one(m_choice_a, m_picker_a_container, m_picker_a_swatch, m_picker_a_label);
    update_one(m_choice_b, m_picker_b_container, m_picker_b_swatch, m_picker_b_label);
    update_one(m_choice_c, m_picker_c_container, m_picker_c_swatch, m_picker_c_label);
    update_one(m_choice_d, m_picker_d_container, m_picker_d_swatch, m_picker_d_label);

    if (m_surface_offset_target_container || m_surface_offset_target_swatch || m_surface_offset_target_label || m_surface_offset_spin) {
        const int a_filament = std::clamp(m_choice_a ? (m_choice_a->GetSelection() + 1) : int(m_mf.component_a), 1, int(std::max<size_t>(1, m_num_physical)));
        const int b_filament = std::clamp(m_choice_b ? (m_choice_b->GetSelection() + 1) : int(m_mf.component_b), 1, int(std::max<size_t>(1, m_num_physical)));
        MixedFilament active_pair = m_mf;
        active_pair.component_a = unsigned(a_filament);
        active_pair.component_b = unsigned(b_filament);
        double signed_bias_value = mixed_filament_single_surface_offset_value(active_pair, m_nozzle_diameters);

        if (m_surface_offset_spin && m_bias_mode_enabled) {
            const double bias_limit = mixed_filament_bias_limit_mm(active_pair, m_nozzle_diameters);
            m_surface_offset_spin->SetRange(-bias_limit, bias_limit);
            signed_bias_value = m_surface_offset_spin->GetValue();
        }

        const int active_filament = signed_bias_value < -EPSILON ? a_filament : b_filament;
        const int color_idx = active_filament - 1;
        const wxColour color = (color_idx >= 0 && size_t(color_idx) < m_palette.size()) ? m_palette[size_t(color_idx)] : wxColour("#26A69A");
        if (m_surface_offset_target_swatch) {
            m_surface_offset_target_swatch->SetBackgroundColour(color);
            m_surface_offset_target_swatch->Refresh();
        }
        if (m_surface_offset_target_label)
            m_surface_offset_target_label->SetLabel(wxString::Format("F%d", active_filament));
        if (m_surface_offset_target_container) {
            m_surface_offset_target_container->Layout();
            m_surface_offset_target_container->Refresh();
        }
    }
}

void MixedFilamentConfigPanel::update_preview()
{
    const bool simple_mode = m_mf.distribution_mode == int(MixedFilament::Simple);
    const bool same_layer_mode = m_mf.distribution_mode == int(MixedFilament::SameLayerPointillisme);
    const std::string normalized_pattern = MixedFilamentManager::normalize_manual_pattern(m_mf.manual_pattern);
    const bool pattern_row_mode = !normalized_pattern.empty();

    std::vector<unsigned int> initial_sequence;
    if (pattern_row_mode) {
        initial_sequence = decode_manual_pattern_ids(normalized_pattern,
                                                     m_mf.component_a,
                                                     m_mf.component_b,
                                                     m_num_physical,
                                                     m_preview_settings.wall_loops);
    } else {
        std::vector<unsigned int> initial_gradient_ids = simple_mode ? std::vector<unsigned int>() : decode_gradient_ids(m_mf.gradient_component_ids);
        if (initial_gradient_ids.size() >= 3)
            initial_sequence = build_weighted_multi_sequence(initial_gradient_ids, *m_selected_weight_state);
        else
            initial_sequence = build_weighted_pair_sequence(m_mf.component_a,
                                                            m_mf.component_b,
                                                            effective_local_z_preview_mix_b_percent(m_mf, m_preview_settings),
                                                            same_layer_mode);

        if (m_blend_selector && initial_gradient_ids.size() >= 3) {
            std::vector<wxColour> corner_colors;
            corner_colors.reserve(initial_gradient_ids.size());
            for (const unsigned int id : initial_gradient_ids) {
                if (id >= 1 && id <= m_palette.size())
                    corner_colors.emplace_back(m_palette[id - 1]);
            }
            if (corner_colors.size() >= 3)
                m_blend_selector->set_multi_preview(corner_colors, *m_selected_weight_state);
        }
    }

    if (m_mix_preview) {
        if (Slic3r::mixed_filament_supports_bias_apparent_color(m_mf, m_preview_settings, m_bias_mode_enabled) &&
            m_mf.component_a >= 1 && m_mf.component_b >= 1 &&
            m_mf.component_a <= m_physical_colors.size() && m_mf.component_b <= m_physical_colors.size()) {
            const auto [apparent_pct_a, apparent_pct_b] =
                Slic3r::mixed_filament_apparent_pair_percentages(m_mf, m_preview_settings, m_nozzle_diameters, m_bias_mode_enabled);
            m_mf.display_color = MixedFilamentManager::blend_color(
                m_physical_colors[size_t(m_mf.component_a - 1)],
                m_physical_colors[size_t(m_mf.component_b - 1)],
                apparent_pct_a,
                apparent_pct_b);
        }

        const std::string bias_summary =
            mixed_filament_apparent_pair_summary(m_mf, m_preview_settings, m_nozzle_diameters, m_bias_mode_enabled);
        const std::string summary = bias_summary.empty() ? summarize_sequence(initial_sequence) : bias_summary;
        std::vector<double> preview_surface_offsets(m_palette.size() + 1, 0.0);
        if (m_bias_mode_enabled && m_mf.component_a >= 1 && m_mf.component_a < preview_surface_offsets.size())
            preview_surface_offsets[m_mf.component_a] = double(m_mf.component_a_surface_offset);
        if (m_bias_mode_enabled && m_mf.component_b >= 1 && m_mf.component_b < preview_surface_offsets.size())
            preview_surface_offsets[m_mf.component_b] = double(m_mf.component_b_surface_offset);
        m_mix_preview->set_data(m_palette, initial_sequence, same_layer_mode, preview_surface_offsets, wxColour(m_mf.display_color),
                                _L("Preview"), summary.empty() ? wxString() : from_u8(summary));
    }
    update_local_z_breakdown();
}

void MixedFilamentConfigPanel::update_local_z_breakdown()
{
    if (!m_breakdown_label)
        return;

    std::vector<int> weights = *m_selected_weight_state;
    const std::vector<unsigned int> ids = decode_gradient_ids(m_mf.gradient_component_ids);
    if (!ids.empty())
        weights = normalize_gradient_weights(weights, ids.size());

    const std::string breakdown = summarize_local_z_breakdown(m_mf, weights, m_preview_settings);
    m_breakdown_label->SetLabel(from_u8(breakdown));
    m_breakdown_label->Wrap(FromDIP(360));
    m_breakdown_label->Show(!breakdown.empty());
    Layout();
}

class MixedFilamentDragHandle : public wxPanel
{
public:
    MixedFilamentDragHandle(wxWindow *parent, const wxColour &dot_color, const wxColour &bg_color)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
        , m_dot_color(dot_color)
    {
        const wxSize handle_size = parent ? parent->FromDIP(wxSize(14, 18)) : wxSize(14, 18);
        SetMinSize(handle_size);
        SetMaxSize(handle_size);
        SetInitialSize(handle_size);
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        SetBackgroundColour(bg_color);
        SetCursor(wxCursor(wxCURSOR_SIZING));
        Bind(wxEVT_PAINT, &MixedFilamentDragHandle::on_paint, this);
    }

    void set_colors(const wxColour &dot_color, const wxColour &bg_color)
    {
        m_dot_color = dot_color;
        SetBackgroundColour(bg_color);
        Refresh();
    }

private:
    void on_paint(wxPaintEvent &)
    {
        wxAutoBufferedPaintDC dc(this);
        dc.SetBackground(wxBrush(GetBackgroundColour()));
        dc.Clear();
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(m_dot_color));

        const wxSize size = GetClientSize();
        const int    radius = std::max(1, FromDIP(1));
        const int    left_x = std::max(radius, size.x / 2 - FromDIP(2));
        const int    right_x = std::min(size.x - radius - 1, size.x / 2 + FromDIP(2));
        const int    top_y = std::max(radius + 1, size.y / 2 - FromDIP(5));
        const int    gap_y = FromDIP(4);

        for (int row = 0; row < 3; ++row) {
            const int y = top_y + row * gap_y;
            dc.DrawCircle(wxPoint(left_x, y), radius);
            dc.DrawCircle(wxPoint(right_x, y), radius);
        }
    }

    wxColour m_dot_color;
};

static std::vector<size_t> build_mixed_filament_ui_indices(const std::vector<MixedFilament> &mixed,
                                                           const std::vector<uint64_t>      &preferred_order)
{
    std::vector<size_t> ordered_indices;
    std::vector<bool>   used(mixed.size(), false);

    for (const uint64_t stable_id : preferred_order) {
        for (size_t idx = 0; idx < mixed.size(); ++idx) {
            const MixedFilament &entry = mixed[idx];
            if (used[idx] || entry.deleted || entry.stable_id != stable_id)
                continue;
            used[idx] = true;
            ordered_indices.emplace_back(idx);
            break;
        }
    }

    for (size_t idx = 0; idx < mixed.size(); ++idx) {
        if (used[idx] || mixed[idx].deleted)
            continue;
        ordered_indices.emplace_back(idx);
    }

    return ordered_indices;
}

void Sidebar::init_color_mix_panel(wxWindow* parent, wxSizer* sizer)
{
    // Title bar
    p->m_panel_color_mix_title = new StaticBox(parent, wxID_ANY, wxDefaultPosition,
                                               wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_NONE);
    p->m_panel_color_mix_title->SetBackgroundColor(wxColour(248, 248, 248));
    p->m_panel_color_mix_title->SetBackgroundColor2(0xF1F1F1);
    p->m_panel_color_mix_title->SetMinSize(wxSize(-1, FromDIP(30)));
    p->m_panel_color_mix_title->SetMaxSize(wxSize(-1, FromDIP(30)));

    p->m_color_mix_icon = new ScalableButton(p->m_panel_color_mix_title, wxID_ANY, "color_palette");
    auto* label = new Label(p->m_panel_color_mix_title, _L("Color Mixing"), LB_PROPAGATE_MOUSE_EVENT);

    p->m_btn_del_color_mix = new ScalableButton(p->m_panel_color_mix_title, wxID_ANY, "delete_filament");
    p->m_btn_add_color_mix = new ScalableButton(p->m_panel_color_mix_title, wxID_ANY, "add_filament");

    auto* h_title = new wxBoxSizer(wxHORIZONTAL);
    auto* white_left_c = new wxPanel(p->m_panel_color_mix_title, wxID_ANY, wxDefaultPosition, wxSize(FromDIP(SidebarProps::ContentMargin()), -1));
    white_left_c->SetBackgroundColour(StateColor::darkModeColorFor(*wxWHITE));
    h_title->Add(white_left_c, 0, wxEXPAND | wxTOP | wxBOTTOM, 0);
    h_title->Add(p->m_color_mix_icon, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, FromDIP(SidebarProps::TitlebarMargin()));
    h_title->AddSpacer(FromDIP(SidebarProps::ElementSpacing()));
    h_title->Add(label, 0, wxALIGN_CENTER_VERTICAL);
    h_title->AddStretchSpacer();
    h_title->Add(p->m_btn_del_color_mix, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(4));
    h_title->Add(p->m_btn_add_color_mix, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(8));
    auto* white_right_c = new wxPanel(p->m_panel_color_mix_title, wxID_ANY, wxDefaultPosition, wxSize(FromDIP(SidebarProps::ContentMargin()), -1));
    white_right_c->SetBackgroundColour(StateColor::darkModeColorFor(*wxWHITE));
    h_title->Add(white_right_c, 0, wxEXPAND | wxTOP | wxBOTTOM, 0);
    p->m_panel_color_mix_title->SetSizer(h_title);
    p->m_panel_color_mix_title->Layout();

    // Scrolled window for content with max height of 3 rows
    p->m_scrolled_color_mix = new wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
    p->m_scrolled_color_mix->SetScrollRate(0, 5);
    p->m_scrolled_color_mix->SetBackgroundColour(StateColor::darkModeColorFor(*wxWHITE));

    // Content panel — match physical filament content panel (sizer set dynamically in update)
    p->m_panel_color_mix_content = new wxPanel(p->m_scrolled_color_mix, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    p->m_panel_color_mix_content->SetBackgroundColour(StateColor::darkModeColorFor(*wxWHITE));

    // Add content panel to scrolled window
    auto* scrolled_sizer = new wxBoxSizer(wxVERTICAL);
    scrolled_sizer->Add(p->m_panel_color_mix_content, 0, wxEXPAND);
    p->m_scrolled_color_mix->SetSizer(scrolled_sizer);

    sizer->Add(p->m_panel_color_mix_title, 0, wxEXPAND, 0);
    sizer->Add(p->m_scrolled_color_mix, 0, wxEXPAND, 0);

    // Add button: open dialog to create new mix
    p->m_btn_add_color_mix->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        PresetBundle* pb = wxGetApp().preset_bundle;
        if (!pb) return;

        auto* co = pb->project_config.option<ConfigOptionStrings>("filament_colour");
        const std::vector<std::string> colors = co ? co->values : std::vector<std::string>{};
        if (colors.size() < 2) return;

        MixedFilamentDialog dlg(wxGetApp().mainframe, colors);
        if (dlg.ShowModal() != wxID_OK) return;

        auto& mgr = pb->mixed_filaments;
        if (mgr.total_filaments(colors.size()) >= MAXIMUM_FILAMENT_NUMBER) return;
        const MixedFilament& r = dlg.GetResult();
        mgr.add_custom_filament(r.component_a, r.component_b, r.mix_b_percent, colors);
        auto& mfs = mgr.mixed_filaments();
        if (!mfs.empty()) {
            mfs.back().distribution_mode       = r.distribution_mode;
            mfs.back().manual_pattern          = r.manual_pattern;
            mfs.back().gradient_component_ids      = r.gradient_component_ids;
            mfs.back().gradient_component_weights  = r.gradient_component_weights;
            mfs.back().ratio_a                     = r.ratio_a;
            mfs.back().ratio_b                     = r.ratio_b;
            mfs.back().local_z_max_sublayers       = r.local_z_max_sublayers;
            mfs.back().gradient_enabled            = r.gradient_enabled;
            mfs.back().gradient_start              = r.gradient_start;
            mfs.back().gradient_end                = r.gradient_end;
            mfs.back().display_color             = r.display_color;
            mfs.back().ui_mode                       = r.ui_mode;
            mfs.back().custom                  = true;
        }
        if (auto* opt = pb->project_config.option<ConfigOptionString>("mixed_filament_definitions"))
            opt->value = mgr.serialize_custom_entries();
        wxGetApp().plater()->post_slice_state_change_update();
        wxGetApp().plater()->on_filaments_change(p->combos_filament.size());
        update_color_mix_panel();
        m_scrolled_sizer->Layout();
    });

    // Delete button: remove last custom entry
    p->m_btn_del_color_mix->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        PresetBundle* pb = wxGetApp().preset_bundle;
        if (!pb) return;

        auto& mgr = pb->mixed_filaments;
        auto& mfs = mgr.mixed_filaments();
        for (int i = static_cast<int>(mfs.size()) - 1; i >= 0; --i) {
            if (mfs[i].custom && !mfs[i].deleted) {
                mfs[i].deleted = true;
                break;
            }
        }
        if (auto* opt = pb->project_config.option<ConfigOptionString>("mixed_filament_definitions"))
            opt->value = mgr.serialize_custom_entries();
        wxGetApp().plater()->post_slice_state_change_update();
        wxGetApp().plater()->on_filaments_change(p->combos_filament.size());
        update_color_mix_panel();
        m_scrolled_sizer->Layout();
    });

    // Initial visibility: hide if fewer than 2 physical filaments
    update_color_mix_panel();
}

void Sidebar::update_color_mix_panel()
{
    if (!p->m_panel_color_mix_content) return;

    auto* co = wxGetApp().preset_bundle
                   ? wxGetApp().preset_bundle->project_config.option<ConfigOptionStrings>("filament_colour")
                   : nullptr;
    const int n_physical = co ? static_cast<int>(co->values.size()) : 0;
    const bool show = (n_physical >= 2);
    p->m_panel_color_mix_title->Show(show);
    p->m_scrolled_color_mix->Show(show);
    if (!show) {
        return;
    }

    wxWindowUpdateLocker no_updates(p->m_panel_color_mix_content);
    p->m_panel_color_mix_content->DestroyChildren();

    auto* preset_bundle = wxGetApp().preset_bundle;
    const size_t num_physical = p->combos_filament.size();

    std::vector<std::string> physical_colors = co->values;
    physical_colors.resize(num_physical, "#26A69A");

    std::vector<double> nozzle_diameters(num_physical, 0.4);
    if (const ConfigOptionFloats* opt = preset_bundle->printers.get_edited_preset().config.option<ConfigOptionFloats>("nozzle_diameter")) {
        const size_t opt_count = opt->values.size();
        if (opt_count > 0)
            for (size_t i = 0; i < num_physical; ++i)
                nozzle_diameters[i] = std::max(0.05, opt->get_at(unsigned(std::min(i, opt_count - 1))));
    }

    float lower_bound = 0.04f, upper_bound = 0.16f;
    if (preset_bundle->project_config.has("mixed_filament_height_lower_bound"))
        lower_bound = std::max(0.01f, float(preset_bundle->project_config.opt_float("mixed_filament_height_lower_bound")));
    if (preset_bundle->project_config.has("mixed_filament_height_upper_bound"))
        upper_bound = std::max(lower_bound, float(preset_bundle->project_config.opt_float("mixed_filament_height_upper_bound")));

    bool local_z_mode = false;
    if (const ConfigOptionBool* opt = preset_bundle->project_config.option<ConfigOptionBool>("dithering_local_z_mode"))
        local_z_mode = opt->value;

    bool component_bias_enabled = false;
    if (const ConfigOptionBool* opt = preset_bundle->project_config.option<ConfigOptionBool>("mixed_filament_component_bias_enabled"))
        component_bias_enabled = opt->value;

    const MixedFilamentPreviewSettings preview_settings {
        0.2f, lower_bound, upper_bound, 0.f, 0.f, local_z_mode, false, 1
    };
    const MixedFilamentDisplayContext display_context {
        num_physical, physical_colors, nozzle_diameters, preview_settings, component_bias_enabled
    };
    preset_bundle->mixed_filaments.set_display_context(display_context);

    auto& mfs = preset_bundle->mixed_filaments.mixed_filaments();
    bool any_visible = false;
    for (const MixedFilament& mf : mfs)
        if (!mf.deleted) { any_visible = true; break; }

    if (!any_visible) {
        p->m_scrolled_color_mix->Hide();
        p->m_btn_del_color_mix->Hide();
        p->m_btn_add_color_mix->SetBitmap_("icon_add_circle");
        m_scrolled_sizer->Layout();
        return;
    }
    p->m_scrolled_color_mix->Show();
    p->m_btn_del_color_mix->Show();
    p->m_btn_add_color_mix->SetBitmap_("add_filament");

    // 2-column grid matching the physical filament panel layout
    auto* grid_sizer = new wxBoxSizer(wxHORIZONTAL);
    auto* col0 = new wxBoxSizer(wxVERTICAL);
    auto* col1 = new wxBoxSizer(wxVERTICAL);
    grid_sizer->Add(col0, 1, wxEXPAND);
    grid_sizer->Add(col1, 1, wxEXPAND);

    int visible_idx = 0;
    for (size_t i = 0; i < mfs.size(); ++i) {
        MixedFilament& mf = mfs[i];
        if (mf.deleted) continue;

        const std::string synced_color = compute_mixed_filament_display_color(mf, display_context);
        if (mf.display_color != synced_color)
            mf.display_color = synced_color;

        const int virtual_id = static_cast<int>(num_physical) + visible_idx + 1;

        // Badge button: colored background + virtual filament number with gradient support
        auto* badge = new MixedFilamentBadge(p->m_panel_color_mix_content, wxID_ANY,
                                             virtual_id, mf, display_context);

        const std::string normalized_pattern_cm = MixedFilamentManager::normalize_manual_pattern(mf.manual_pattern);
        std::vector<unsigned int> gradient_ids = MixedFilamentManager::decode_gradient_component_ids(mf.gradient_component_ids, 0);
        const bool z_gradient_tile = mf.gradient_enabled && mf.component_a != mf.component_b
                                  && normalized_pattern_cm.empty() && gradient_ids.size() < 3;
        wxString lbl;
        if (!normalized_pattern_cm.empty())
            lbl = wxString(summarize_cycle_pattern_text(normalized_pattern_cm, mf, int(num_physical)));
        else if (gradient_ids.size() >= 3) {
            // parse weights
            const size_t n = gradient_ids.size();
            std::vector<int> weights;
            {
                std::string token;
                for (const char c : mf.gradient_component_weights) {
                    if (c >= '0' && c <= '9') { token.push_back(c); continue; }
                    if (!token.empty()) { weights.emplace_back(std::max(0, std::atoi(token.c_str()))); token.clear(); }
                }
                if (!token.empty()) weights.emplace_back(std::max(0, std::atoi(token.c_str())));
                if (weights.size() != n) weights.assign(n, int(100 / n));
            }
            // normalize to 100
            int sum = 0; for (int v : weights) sum += v;
            if (sum <= 0) { weights.assign(n, 0); weights[0] = 100; sum = 100; }
            for (size_t k = 0; k < n; ++k) {
                const unsigned int fid = gradient_ids[k];
                const int pct = int(std::round(100.0 * weights[k] / sum));
                if (k > 0) lbl += "+";
                lbl += wxString::Format("F%u %d%%", fid, pct);
            }
        } else if (z_gradient_tile) {
            const unsigned from_id =
                mf.gradient_start >= mf.gradient_end ? mf.component_a : mf.component_b;
            const unsigned to_id =
                mf.gradient_start >= mf.gradient_end ? mf.component_b : mf.component_a;
            lbl = wxString::Format("F%u->F%u", from_id, to_id);
        } else {
            const int pct_b = std::clamp(mf.mix_b_percent, 0, 100);
            const int pct_a = 100 - pct_b;
            lbl = wxString::Format("F%u %d%%+F%u %d%%", mf.component_a, pct_a, mf.component_b, pct_b);
            if (mf.distribution_mode != int(MixedFilament::Simple))
                for (unsigned int fid : gradient_ids)
                    lbl += wxString::Format("+F%u", fid);
        }
        
        bool has_error = !is_filament_compatible(mf);
        
        // Create a panel with border for the text
        auto* name_panel = new wxPanel(p->m_panel_color_mix_content, wxID_ANY);
        name_panel->SetBackgroundColour(StateColor::darkModeColorFor(*wxWHITE));

        auto* name_sizer = new wxBoxSizer(wxHORIZONTAL);

        // Add error icon if there's an error
        if (has_error) {
            name_sizer->AddSpacer(FromDIP(8));
            ScalableBitmap error_bmp(name_panel, "error_icon_red_exclamation", 14);
            auto* error_icon = new wxStaticBitmap(name_panel, wxID_ANY, error_bmp.bmp());
            name_sizer->Add(error_icon, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(4));
        }

        auto* name_btn = new wxStaticText(name_panel, wxID_ANY, lbl, wxDefaultPosition, wxDefaultSize, 0);
        name_btn->SetBackgroundColour(StateColor::darkModeColorFor(*wxWHITE));
        name_btn->SetForegroundColour(StateColor::darkModeColorFor(wxColour("#262E30")));
        name_btn->SetCursor(wxCursor(wxCURSOR_HAND));
        name_btn->SetMinSize(wxSize(0, -1)); // allow sizer to shrink below text width

        int name_flags = wxEXPAND | (has_error ? (wxTOP | wxBOTTOM | wxRIGHT) : wxALL);
        name_sizer->Add(name_btn, 1, name_flags, FromDIP(8));
        name_panel->SetSizer(name_sizer);
        name_panel->SetMinSize(wxSize(name_panel->FromDIP(100), name_panel->FromDIP(30)));
        name_panel->SetMaxSize(wxSize(-1, name_panel->FromDIP(30)));

        // Use wxControl::Ellipsize on resize to match combo truncation behavior
        name_panel->Bind(wxEVT_SIZE, [lbl, name_btn, name_panel](wxSizeEvent& evt) {
            name_panel->Layout(); // force sizer layout so name_btn has its current size
            int avail = name_btn->GetSize().x;
            if (avail > 0) {
                wxClientDC dc(name_btn);
                dc.SetFont(name_btn->GetFont());
                wxString ellipsized = wxControl::Ellipsize(lbl, dc, wxELLIPSIZE_END, avail);
                name_btn->SetLabel(ellipsized);
            }
            evt.Skip();
        });

        // Add border to the panel
        name_panel->Bind(wxEVT_PAINT, [](wxPaintEvent& evt) {
            wxPanel* panel = dynamic_cast<wxPanel*>(evt.GetEventObject());
            if (!panel) return;

            wxPaintDC dc(panel);
            wxRect rect = panel->GetClientRect();

            // Draw border matching combo (#dbdbdb, 1px)
            dc.SetPen(wxPen(StateColor::darkModeColorFor(wxColour(0xdb, 0xdb, 0xdb)), 1));
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.DrawRectangle(rect);
        });

        name_btn->SetToolTip(lbl);
        name_btn->Bind(wxEVT_LEFT_DOWN, [this, i](wxMouseEvent&) {
            auto* co = wxGetApp().preset_bundle->project_config.option<ConfigOptionStrings>("filament_colour");
            const std::vector<std::string> colors = co ? co->values : std::vector<std::string>{};
            if (colors.size() < 2) return;
            auto& mgr = wxGetApp().preset_bundle->mixed_filaments;
            auto& mfs2 = mgr.mixed_filaments();
            if (i >= mfs2.size()) return;
            MixedFilamentDialog dlg(wxGetApp().mainframe, colors, mfs2[i]);
            if (dlg.ShowModal() != wxID_OK) return;
            const MixedFilament& r = dlg.GetResult();
            mfs2[i].component_a                = r.component_a;
            mfs2[i].component_b                = r.component_b;
            mfs2[i].mix_b_percent              = r.mix_b_percent;
            mfs2[i].distribution_mode          = r.distribution_mode;
            mfs2[i].manual_pattern             = r.manual_pattern;
            mfs2[i].gradient_component_ids     = r.gradient_component_ids;
            mfs2[i].gradient_component_weights = r.gradient_component_weights;
            mfs2[i].ratio_a                    = r.ratio_a;
            mfs2[i].ratio_b                    = r.ratio_b;
            mfs2[i].local_z_max_sublayers      = r.local_z_max_sublayers;
            mfs2[i].gradient_enabled           = r.gradient_enabled;
            mfs2[i].gradient_start             = r.gradient_start;
            mfs2[i].gradient_end               = r.gradient_end;
            mfs2[i].display_color               = r.display_color;
            mfs2[i].ui_mode                       = r.ui_mode;
            mfs2[i].custom                      = true;
            if (auto* opt = wxGetApp().preset_bundle->project_config.option<ConfigOptionString>("mixed_filament_definitions"))
                opt->value = mgr.serialize_custom_entries();
            wxGetApp().plater()->post_slice_state_change_update();
            wxGetApp().plater()->on_filaments_change(p->combos_filament.size());
            wxWeakRef<Sidebar> weak_this(this);
            wxTheApp->CallAfter([weak_this]() {
                Sidebar* sidebar = weak_this.get();
                if (sidebar) {
                    sidebar->update_color_mix_panel();
                    sidebar->m_scrolled_sizer->Layout();
                }
            });
        });

        auto* menu_btn = new ScalableButton(p->m_panel_color_mix_content, wxID_ANY, "menu_filament");
        menu_btn->SetToolTip(_L("Options"));
        menu_btn->Bind(wxEVT_BUTTON, [this, i, visible_idx, num_physical, menu_btn](wxCommandEvent&) {
            wxMenu menu;
            const int edit_id = wxWindow::NewControlId();
            const int del_id  = wxWindow::NewControlId();
            const int merge_to_id = wxWindow::NewControlId();
            
            menu.Append(edit_id, _L("Edit"));
            menu.Bind(wxEVT_MENU, [this, i](wxCommandEvent&) {
                auto* co = wxGetApp().preset_bundle->project_config.option<ConfigOptionStrings>("filament_colour");
                const std::vector<std::string> colors = co ? co->values : std::vector<std::string>{};
                if (colors.size() < 2) return;
                auto& mgr = wxGetApp().preset_bundle->mixed_filaments;
                auto& mfs2 = mgr.mixed_filaments();
                if (i >= mfs2.size()) return;
                MixedFilamentDialog dlg(wxGetApp().mainframe, colors, mfs2[i]);
                if (dlg.ShowModal() != wxID_OK) return;
                const MixedFilament& r = dlg.GetResult();
                mfs2[i].component_a                = r.component_a;
                mfs2[i].component_b                = r.component_b;
                mfs2[i].mix_b_percent              = r.mix_b_percent;
                mfs2[i].distribution_mode          = r.distribution_mode;
                mfs2[i].manual_pattern             = r.manual_pattern;
                mfs2[i].gradient_component_ids     = r.gradient_component_ids;
                mfs2[i].gradient_component_weights = r.gradient_component_weights;
                mfs2[i].ratio_a                    = r.ratio_a;
                mfs2[i].ratio_b                    = r.ratio_b;
                mfs2[i].local_z_max_sublayers      = r.local_z_max_sublayers;
                mfs2[i].gradient_enabled           = r.gradient_enabled;
                mfs2[i].gradient_start             = r.gradient_start;
                mfs2[i].gradient_end               = r.gradient_end;
                mfs2[i].ui_mode                       = r.ui_mode;
                mfs2[i].custom                      = true;
                if (auto* opt = wxGetApp().preset_bundle->project_config.option<ConfigOptionString>("mixed_filament_definitions"))
                    opt->value = mgr.serialize_custom_entries();
                wxGetApp().plater()->post_slice_state_change_update();
                wxGetApp().plater()->on_filaments_change(p->combos_filament.size());
                wxWeakRef<Sidebar> weak_this(this);
                wxTheApp->CallAfter([weak_this]() {
                    Sidebar* sidebar = weak_this.get();
                    if (sidebar) {
                        sidebar->update_color_mix_panel();
                        sidebar->m_scrolled_sizer->Layout();
                    }
                });
            }, edit_id);

            // Add "Merge with" submenu - allows merging to any other filament (physical or mixed)
            // Build list of all available target filaments
            wxMenu* merge_submenu = new wxMenu();
            
            // Get physical filament icons
            std::vector<wxBitmap*> icons = get_extruder_color_icons(true);
            
            // Add physical filaments as targets
            for (size_t phys_idx = 0; phys_idx < num_physical; ++phys_idx) {
                const int target_id = wxWindow::NewControlId();
                auto preset = wxGetApp().preset_bundle->filaments.find_preset(wxGetApp().preset_bundle->filament_presets[phys_idx]);
                wxString target_label = preset ? from_u8(preset->label(false)) : wxString::Format(_L("Filament %d"), phys_idx + 1);
                
                // Use icon if available
                wxMenuItem* item = nullptr;
                if (phys_idx < icons.size() && icons[phys_idx]) {
                    item = new wxMenuItem(merge_submenu, target_id, target_label);
                    item->SetBitmap(*icons[phys_idx]);
                    merge_submenu->Append(item);
                } else {
                    merge_submenu->Append(target_id, target_label);
                }
                
                merge_submenu->Bind(wxEVT_MENU, [this, visible_idx, phys_idx, num_physical](wxCommandEvent&) {
                    // Source: mixed filament with visible_idx (0-based in visible list)
                    // Target: physical filament phys_idx (0-based)
                    // Mixed filament virtual ID = num_physical + visible_idx + 1 (1-based), convert to 0-based
                    size_t source_virtual_id = num_physical + visible_idx;
                    change_filament(source_virtual_id, phys_idx);
                }, target_id);
            }
            
            // Add other mixed filaments as targets
            auto& mgr = wxGetApp().preset_bundle->mixed_filaments;
            auto& mfs_for_menu = mgr.mixed_filaments();
            const size_t total_mixed = mfs_for_menu.size();
            
            // Get icon dimensions for mixed filaments
            const double em = Slic3r::GUI::wxGetApp().em_unit();
            const int icon_width = lround(2 * em);
            const int icon_height = lround(2 * em);
            
            size_t target_visible_idx = 0;
            for (size_t j = 0; j < total_mixed; ++j) {
                if (mfs_for_menu[j].deleted) continue;
                
                // Skip self (compare by visible index)
                if (target_visible_idx == visible_idx) {
                    target_visible_idx++;
                    continue;
                }
                
                const int target_virtual_id = static_cast<int>(num_physical) + target_visible_idx + 1;
                const wxString target_label = wxString::Format(_L("Mixed Filament %d"), target_virtual_id);
                const int target_id = wxWindow::NewControlId();
                
                // Create colored bitmap for mixed filament — gradient filaments get a gradient icon
                MixedFilamentDisplayContext menu_ctx;
                {
                    auto* co2 = wxGetApp().preset_bundle->project_config.option<ConfigOptionStrings>("filament_colour");
                    menu_ctx.physical_colors = co2 ? co2->values : std::vector<std::string>{};
                    menu_ctx.num_physical = num_physical;
                }
                wxBitmap* mixed_bmp = create_mixed_filament_menu_bitmap(
                    mfs_for_menu[j], menu_ctx, icon_width, icon_height,
                    wxString::Format("%d", target_virtual_id));

                wxMenuItem* item = new wxMenuItem(merge_submenu, target_id, target_label);
                item->SetBitmap(*mixed_bmp);
                merge_submenu->Append(item);
                
                merge_submenu->Bind(wxEVT_MENU, [this, visible_idx, target_visible_idx, num_physical](wxCommandEvent&) {
                    // Source: mixed filament with visible_idx (0-based in visible list)
                    // Target: mixed filament with target_visible_idx (0-based in visible list)
                    // Virtual ID (0-based) = num_physical + visible_idx
                    size_t source_virtual_id = num_physical + visible_idx;
                    size_t target_virtual_id = num_physical + target_visible_idx;
                    change_filament(source_virtual_id, target_virtual_id);
                }, target_id);
                
                target_visible_idx++;
            }
            
            menu.AppendSubMenu(merge_submenu, _L("Merge with"));
            
            menu.Append(del_id, _L("Delete"));
            menu.Bind(wxEVT_MENU, [this, i, num_physical](wxCommandEvent&) {
                auto& mgr2 = wxGetApp().preset_bundle->mixed_filaments;
                auto& mfs2 = mgr2.mixed_filaments();
                const std::vector<MixedFilament> old_mixed = mfs2;
                if (i < mfs2.size()) { mfs2[i].deleted = true; mfs2[i].enabled = false; }
                if (auto* opt = wxGetApp().preset_bundle->project_config.option<ConfigOptionString>("mixed_filament_definitions"))
                    opt->value = mgr2.serialize_custom_entries();
                wxGetApp().preset_bundle->update_mixed_filament_id_remap(old_mixed, num_physical, num_physical, i);
                wxGetApp().plater()->post_slice_state_change_update();
                wxGetApp().plater()->on_filaments_change(num_physical);
                wxWeakRef<Sidebar> weak_this(this);
                wxTheApp->CallAfter([weak_this]() {
                    Sidebar* sidebar = weak_this.get();
                    if (sidebar) {
                        sidebar->update_color_mix_panel();
                        sidebar->m_scrolled_sizer->Layout();
                    }
                });
            }, del_id);
            
            wxPoint pt{0, menu_btn->GetSize().GetHeight()};
            pt = menu_btn->ClientToScreen(pt);
            pt = wxGetApp().mainframe->ScreenToClient(pt);
            wxGetApp().mainframe->PopupMenu(&menu, pt);
        });

        auto* cell = new wxBoxSizer(wxHORIZONTAL);
        // Match physical filament asymmetric layout: left column gets left spacer, right doesn't
        if (visible_idx % 2 == 0)
            cell->AddSpacer(FromDIP(SidebarProps::ContentMargin()));
        cell->Add(badge,    0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(SidebarProps::ElementSpacing()) - FromDIP(2));
        cell->Add(name_panel, 1, wxEXPAND | wxALL, FromDIP(2));
        cell->Add(menu_btn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, FromDIP(SidebarProps::ElementSpacing()) - FromDIP(2));
        cell->AddSpacer(FromDIP(SidebarProps::ContentMargin()));

        (visible_idx % 2 == 0 ? col0 : col1)->Add(cell, 0, wxEXPAND | wxBOTTOM, FromDIP(4));

        ++visible_idx;
    }

    // If odd count, pad right column so left column doesn't stretch
    if (visible_idx % 2 == 1)
        col1->AddStretchSpacer(1);

    // Wrap grid in vertical sizer for padding, then set as panel sizer (matches physical structure)
    auto* wrapper = new wxBoxSizer(wxVERTICAL);
    wrapper->Add(grid_sizer, 0, wxEXPAND | wxTOP | wxBOTTOM, FromDIP(8));
    p->m_panel_color_mix_content->SetSizer(wrapper);
    p->m_panel_color_mix_content->Layout();

    // Dynamic height: grow with rows up to 3, only cap when > 3 rows
    const wxSize content_best = p->m_panel_color_mix_content->GetBestSize();
    const int row_count = (visible_idx + 1) / 2; // rows (2 columns)
    const int desired_h = row_count > 3
        ? (content_best.GetHeight() / row_count) * 3
        : content_best.GetHeight();
    p->m_scrolled_color_mix->SetMinSize({-1, desired_h});
    p->m_scrolled_color_mix->SetMaxSize({-1, desired_h});
    
    m_scrolled_sizer->Layout();
    wxWeakRef<Sidebar> weak_this(this);
    wxTheApp->CallAfter([weak_this]() {
        Sidebar* sidebar = weak_this.get();
        if (sidebar && sidebar->p && sidebar->p->m_scrolled_color_mix) {
            int vh = sidebar->p->m_scrolled_color_mix->GetVirtualSize().y;
            int ch = sidebar->p->m_scrolled_color_mix->GetClientSize().y;
            sidebar->p->m_scrolled_color_mix->Scroll(0, std::max(0, vh - ch));
        }
    });
    p->m_panel_color_mix_content->Refresh();

    // Disable add buttons when combined filament limit reached
    if (preset_bundle) {
        const bool can_add = preset_bundle->mixed_filaments.total_filaments(num_physical) < MAXIMUM_FILAMENT_NUMBER;
        if (p->m_bpButton_add_filament)
            p->m_bpButton_add_filament->Enable(can_add);
        if (p->m_btn_add_color_mix)
            p->m_btn_add_color_mix->Enable(can_add);
    }
}

void Sidebar::update_mixed_filament_panel(bool sync_manager)
{
    // Check for new collapsible structure
    if (!p->m_panel_mixed_filaments_title || !p->m_panel_mixed_filaments_content)
        return;

    wxWindowUpdateLocker noUpdates_sidebar(this);
    wxWindowUpdateLocker noUpdates_mixed_panel(p->m_panel_mixed_filaments_content);

    auto refresh_model_canvas_colors = []() {
        Plater *plater = wxGetApp().plater();
        if (plater == nullptr)
            return;

        auto refresh_canvas = [](GLCanvas3D *canvas) {
            if (canvas == nullptr || !canvas->is_initialized())
                return;
            canvas->update_volumes_colors_by_extruder();
            canvas->render();
        };

        refresh_canvas(plater->get_view3D_canvas3D());
        refresh_canvas(plater->get_assmeble_canvas3D());
    };

    int prev_rows_view_y = 0;
    for (wxWindow *child : p->m_panel_mixed_filaments_content->GetChildren()) {
        if (auto *scrolled = dynamic_cast<wxScrolledWindow*>(child)) {
            int tmp_x = 0;
            scrolled->GetViewStart(&tmp_x, &prev_rows_view_y);
            break;
        }
    }

    auto *preset_bundle = wxGetApp().preset_bundle;
    if (!preset_bundle)
        return;
    DynamicPrintConfig *print_cfg = &preset_bundle->prints.get_edited_preset().config;

    const size_t num_physical = p->combos_filament.size();
    ConfigOptionStrings *color_opt = preset_bundle->project_config.option<ConfigOptionStrings>("filament_colour");
    std::vector<std::string> physical_colors = color_opt ? color_opt->values : std::vector<std::string>();
    physical_colors.resize(num_physical, "#26A69A");
    std::vector<double> nozzle_diameters(num_physical, 0.4);
    if (const ConfigOptionFloats *opt = preset_bundle->printers.get_edited_preset().config.option<ConfigOptionFloats>("nozzle_diameter")) {
        const size_t opt_count = opt->values.size();
        if (opt_count > 0) {
            for (size_t i = 0; i < num_physical; ++i)
                nozzle_diameters[i] = std::max(0.05, opt->get_at(unsigned(std::min(i, opt_count - 1))));
        }
    }

    auto get_mixed_bool = [preset_bundle, print_cfg](const std::string &key, bool fallback) {
        if (const ConfigOptionBool *opt = preset_bundle->project_config.option<ConfigOptionBool>(key))
            return opt->value;
        if (const ConfigOptionInt *opt = preset_bundle->project_config.option<ConfigOptionInt>(key))
            return opt->value != 0;
        if (print_cfg) {
            if (const ConfigOptionBool *opt = print_cfg->option<ConfigOptionBool>(key))
                return opt->value;
            if (const ConfigOptionInt *opt = print_cfg->option<ConfigOptionInt>(key))
                return opt->value != 0;
        }
        return fallback;
    };
    auto get_mixed_mode = [preset_bundle, print_cfg](bool fallback) {
        if (const ConfigOptionBool *opt = preset_bundle->project_config.option<ConfigOptionBool>("mixed_filament_gradient_mode"))
            return opt->value;
        if (const ConfigOptionInt *opt = preset_bundle->project_config.option<ConfigOptionInt>("mixed_filament_gradient_mode"))
            return opt->value != 0;
        if (print_cfg) {
            if (const ConfigOptionBool *opt = print_cfg->option<ConfigOptionBool>("mixed_filament_gradient_mode"))
                return opt->value;
            if (const ConfigOptionInt *opt = print_cfg->option<ConfigOptionInt>("mixed_filament_gradient_mode"))
                return opt->value != 0;
        }
        return fallback;
    };
    auto get_mixed_float = [preset_bundle, print_cfg](const std::string &key, float fallback) {
        if (preset_bundle->project_config.has(key))
            return float(preset_bundle->project_config.opt_float(key));
        if (print_cfg && print_cfg->has(key))
            return float(print_cfg->opt_float(key));
        return fallback;
    };
    auto get_mixed_string = [preset_bundle, print_cfg](const std::string &key, const std::string &fallback = std::string()) {
        std::string project_value;
        if (preset_bundle->project_config.has(key))
            project_value = preset_bundle->project_config.opt_string(key);
        if (!project_value.empty())
            return project_value;
        if (print_cfg && print_cfg->has(key)) {
            const std::string print_value = print_cfg->opt_string(key);
            if (!print_value.empty())
                return print_value;
        }
        return project_value.empty() ? fallback : project_value;
    };
    auto set_mixed_float = [preset_bundle, print_cfg](const std::string &key, float value) {
        if (print_cfg) {
            if (ConfigOptionFloat *opt = print_cfg->option<ConfigOptionFloat>(key))
                opt->value = value;
        }
        if (ConfigOptionFloat *opt = preset_bundle->project_config.option<ConfigOptionFloat>(key))
            opt->value = value;
        else
            preset_bundle->project_config.set_key_value(key, new ConfigOptionFloat(value));
    };
    auto set_mixed_string = [preset_bundle, print_cfg](const std::string &key, const std::string &value) {
        if (print_cfg) {
            if (ConfigOptionString *opt = print_cfg->option<ConfigOptionString>(key))
                opt->value = value;
        }
        if (ConfigOptionString *opt = preset_bundle->project_config.option<ConfigOptionString>(key))
            opt->value = value;
        else
            preset_bundle->project_config.set_key_value(key, new ConfigOptionString(value));
    };
    auto set_mixed_bool = [preset_bundle, print_cfg](const std::string &key, bool value) {
        if (print_cfg) {
            if (ConfigOptionBool *opt = print_cfg->option<ConfigOptionBool>(key))
                opt->value = value;
            else if (ConfigOptionInt *opt = print_cfg->option<ConfigOptionInt>(key))
                opt->value = value ? 1 : 0;
        }
        if (ConfigOptionBool *opt = preset_bundle->project_config.option<ConfigOptionBool>(key))
            opt->value = value;
        else if (ConfigOptionInt *opt = preset_bundle->project_config.option<ConfigOptionInt>(key))
            opt->value = value ? 1 : 0;
        else
            preset_bundle->project_config.set_key_value(key, new ConfigOptionBool(value));
    };
    auto set_mixed_mode = [preset_bundle, print_cfg](bool enabled) {
        if (print_cfg) {
            if (ConfigOptionBool *opt = print_cfg->option<ConfigOptionBool>("mixed_filament_gradient_mode"))
                opt->value = enabled;
            else if (ConfigOptionInt *opt = print_cfg->option<ConfigOptionInt>("mixed_filament_gradient_mode"))
                opt->value = enabled ? 1 : 0;
        }
        if (ConfigOptionBool *opt = preset_bundle->project_config.option<ConfigOptionBool>("mixed_filament_gradient_mode"))
            opt->value = enabled;
        else if (ConfigOptionInt *opt = preset_bundle->project_config.option<ConfigOptionInt>("mixed_filament_gradient_mode"))
            opt->value = enabled ? 1 : 0;
        else
            preset_bundle->project_config.set_key_value("mixed_filament_gradient_mode", new ConfigOptionBool(enabled));
    };
    auto notify_mixed_change = [print_cfg]() {
        if (!print_cfg)
            return;
        if (auto *print_tab = wxGetApp().get_tab(Preset::TYPE_PRINT))
            print_tab->update_dirty();
        if (wxGetApp().mainframe)
            wxGetApp().mainframe->on_config_changed(print_cfg);
    };
    auto decode_gradient_ids = [](const std::string &encoded) {
        return MixedFilamentManager::decode_gradient_component_ids(encoded, 0);
    };
    auto encode_gradient_ids = [](const std::vector<unsigned int> &ids) {
        return MixedFilamentManager::encode_gradient_component_ids(ids);
    };
    auto decode_gradient_weights = [](const std::string &encoded, size_t expected_count) {
        std::vector<int> out;
        if (encoded.empty() || expected_count == 0)
            return out;
        std::string token;
        for (const char c : encoded) {
            if (c >= '0' && c <= '9') {
                token.push_back(c);
                continue;
            }
            if (!token.empty()) {
                out.emplace_back(std::max(0, std::atoi(token.c_str())));
                token.clear();
            }
        }
        if (!token.empty())
            out.emplace_back(std::max(0, std::atoi(token.c_str())));
        if (out.size() != expected_count)
            out.clear();
        return out;
    };
    auto normalize_gradient_weights = [](const std::vector<int> &weights, size_t n) {
        std::vector<int> out = weights;
        if (out.size() != n)
            out.assign(n, (n > 0) ? int(100 / n) : 0);
        int sum = 0;
        for (int &v : out) {
            v = std::max(0, v);
            sum += v;
        }
        if (sum <= 0 && n > 0) {
            out.assign(n, 0);
            out[0] = 100;
            return out;
        }
        std::vector<double> rem(n, 0.);
        int assigned = 0;
        for (size_t i = 0; i < n; ++i) {
            const double exact = 100.0 * double(out[i]) / double(sum);
            out[i] = int(std::floor(exact));
            rem[i] = exact - double(out[i]);
            assigned += out[i];
        }
        int missing = std::max(0, 100 - assigned);
        while (missing > 0) {
            size_t best_idx = 0;
            double best_rem = -1.0;
            for (size_t i = 0; i < rem.size(); ++i) {
                if (rem[i] > best_rem) {
                    best_rem = rem[i];
                    best_idx = i;
                }
            }
            ++out[best_idx];
            rem[best_idx] = 0.0;
            --missing;
        }
        return out;
    };
    auto encode_gradient_weights = [](const std::vector<int> &weights) {
        std::ostringstream ss;
        for (size_t i = 0; i < weights.size(); ++i) {
            if (i > 0)
                ss << '/';
            ss << std::max(0, weights[i]);
        }
        return ss.str();
    };
    auto build_weighted_multi_sequence = [normalize_gradient_weights](const std::vector<unsigned int> &ids,
                                                                      const std::vector<int> &weights,
                                                                      size_t max_cycle_limit) {
        if (ids.empty())
            return std::vector<unsigned int>();

        std::vector<unsigned int> filtered_ids;
        std::vector<int> counts;
        filtered_ids.reserve(ids.size());
        counts.reserve(ids.size());

        std::vector<int> normalized = normalize_gradient_weights(weights, ids.size());
        for (size_t i = 0; i < ids.size(); ++i) {
            const int weight = (i < normalized.size()) ? std::max(0, normalized[i]) : 0;
            if (weight <= 0)
                continue;
            filtered_ids.emplace_back(ids[i]);
            counts.emplace_back(weight);
        }
        if (filtered_ids.empty()) {
            filtered_ids = ids;
            counts.assign(ids.size(), 1);
        }

        int g = 0;
        for (const int c : counts)
            g = std::gcd(g, std::max(1, c));
        if (g > 1) {
            for (int &c : counts)
                c = std::max(1, c / g);
        }

        constexpr size_t k_max_cycle = 48;
        const size_t effective_cycle_limit =
            max_cycle_limit > 0 ? std::min(k_max_cycle, std::max<size_t>(1, max_cycle_limit)) : k_max_cycle;
        reduce_weight_counts_to_cycle_limit(counts, effective_cycle_limit);

        std::vector<unsigned int> reduced_ids;
        std::vector<int> reduced_counts;
        reduced_ids.reserve(filtered_ids.size());
        reduced_counts.reserve(counts.size());
        for (size_t i = 0; i < counts.size(); ++i) {
            if (counts[i] <= 0)
                continue;
            reduced_ids.emplace_back(filtered_ids[i]);
            reduced_counts.emplace_back(counts[i]);
        }
        if (reduced_ids.empty())
            return std::vector<unsigned int>();
        filtered_ids = std::move(reduced_ids);
        counts = std::move(reduced_counts);

        const int total = std::accumulate(counts.begin(), counts.end(), 0);
        if (total <= 0)
            return std::vector<unsigned int>(filtered_ids.begin(), filtered_ids.end());

        const size_t cycle = size_t(total);

        std::vector<unsigned int> sequence;
        sequence.reserve(cycle);
        std::vector<int> emitted(counts.size(), 0);
        for (size_t pos = 0; pos < cycle; ++pos) {
            size_t best_idx = 0;
            double best_score = -1e9;
            for (size_t i = 0; i < counts.size(); ++i) {
                const double target = double(pos + 1) * double(counts[i]) / double(total);
                const double score = target - double(emitted[i]);
                if (score > best_score) {
                    best_score = score;
                    best_idx = i;
                }
            }
            ++emitted[best_idx];
            sequence.emplace_back(filtered_ids[best_idx]);
        }
        if (sequence.empty())
            sequence = filtered_ids;
        return sequence;
    };
    auto decode_manual_pattern_ids = [num_physical](const std::string &pattern,
                                                    unsigned int       component_a,
                                                    unsigned int       component_b,
                                                    size_t             wall_loops) {
        return build_grouped_manual_pattern_preview_sequence(pattern, component_a, component_b, num_physical, wall_loops);
    };
    const bool height_weighted_mode = get_mixed_mode(false);
    int   gradient_mode = height_weighted_mode ? 1 : 0;
    float lower_bound   = std::max(0.01f, get_mixed_float("mixed_filament_height_lower_bound", 0.04f));
    float upper_bound   = std::max(lower_bound, get_mixed_float("mixed_filament_height_upper_bound", 0.16f));
    float preferred_local_z_a = std::max(0.f, get_mixed_float("mixed_color_layer_height_a", 0.f));
    float preferred_local_z_b = std::max(0.f, get_mixed_float("mixed_color_layer_height_b", 0.f));
    float nominal_layer_height = 0.2f;
    if (print_cfg && print_cfg->has("layer_height"))
        nominal_layer_height = float(print_cfg->opt_float("layer_height"));
    nominal_layer_height = std::max(0.01f, nominal_layer_height);
    size_t wall_loops = 1;
    if (print_cfg && print_cfg->has("wall_loops"))
        wall_loops = std::max<size_t>(1, size_t(std::max(1, print_cfg->opt_int("wall_loops"))));
    const bool local_z_mode = get_mixed_bool("dithering_local_z_mode", false);
    const bool local_z_direct_multicolor =
        get_mixed_bool("dithering_local_z_direct_multicolor", false) &&
        preferred_local_z_a <= EPSILON &&
        preferred_local_z_b <= EPSILON;
    const bool component_bias_enabled = get_mixed_bool("mixed_filament_component_bias_enabled", false);
    float pointillism_pixel_size = std::max(0.f, get_mixed_float("mixed_filament_pointillism_pixel_size", 0.f));
    float pointillism_line_gap   = std::max(0.f, get_mixed_float("mixed_filament_pointillism_line_gap", 0.f));
    float mixed_surface_indentation = std::clamp(get_mixed_float("mixed_filament_surface_indentation", 0.f), -2.f, 2.f);
    bool  advanced_dithering = get_mixed_bool("mixed_filament_advanced_dithering", false);
    const std::string mixed_definitions = get_mixed_string("mixed_filament_definitions");
    const MixedFilamentPreviewSettings preview_settings {
        nominal_layer_height,
        lower_bound,
        upper_bound,
        preferred_local_z_a,
        preferred_local_z_b,
        local_z_mode,
        local_z_direct_multicolor,
        wall_loops
    };
    const MixedFilamentDisplayContext display_context {
        num_physical,
        physical_colors,
        nozzle_diameters,
        preview_settings,
        component_bias_enabled
    };
    auto summarize_sequence = [num_physical](const std::vector<unsigned int> &sequence) {
        if (sequence.empty() || num_physical == 0)
            return std::string();
        std::vector<size_t> counts(num_physical + 1, size_t(0));
        size_t total = 0;
        for (const unsigned int id : sequence) {
            if (id == 0 || id > num_physical)
                continue;
            ++counts[id];
            ++total;
        }
        if (total == 0)
            return std::string();
        std::ostringstream ss;
        bool first = true;
        for (size_t id = 1; id <= num_physical; ++id) {
            if (counts[id] == 0)
                continue;
            const int pct = int(std::lround(100.0 * double(counts[id]) / double(total)));
            if (!first)
                ss << "  ";
            first = false;
            ss << "F" << id << ":" << pct << "%";
        }
        return ss.str();
    };
    auto compute_entry_display_color = [display_context](const MixedFilament &entry) {
        return compute_mixed_filament_display_color(entry, display_context);
    };

    auto &mixed_mgr = preset_bundle->mixed_filaments;
    mixed_mgr.set_display_context(display_context);
    if (sync_manager) {
        mixed_mgr.auto_generate(physical_colors);
        mixed_mgr.clear_custom_entries();
        mixed_mgr.load_custom_entries(mixed_definitions, physical_colors);
        mixed_mgr.apply_gradient_settings(gradient_mode, lower_bound, upper_bound, advanced_dithering);
    }

    if (component_bias_enabled) {
        for (MixedFilament &entry : mixed_mgr.mixed_filaments()) {
            const float bias_value = mixed_filament_single_surface_offset_value(entry, nozzle_diameters);
            const auto balanced_pair = mixed_filament_single_surface_offset_pair(entry, bias_value, nozzle_diameters);
            entry.component_a_surface_offset = balanced_pair.first;
            entry.component_b_surface_offset = balanced_pair.second;
        }
    }

    // During project load, sidebar may refresh before physical filament combos
    // finish syncing. Avoid overwriting persisted mixed definitions while the
    // physical filament set is incomplete.
    if (num_physical >= 2) {
        set_mixed_mode(height_weighted_mode);
        set_mixed_bool("mixed_filament_component_bias_enabled", component_bias_enabled);
        set_mixed_float("mixed_filament_height_lower_bound", lower_bound);
        set_mixed_float("mixed_filament_height_upper_bound", upper_bound);
        set_mixed_float("mixed_color_layer_height_a", preferred_local_z_a);
        set_mixed_float("mixed_color_layer_height_b", preferred_local_z_b);
        set_mixed_float("mixed_filament_pointillism_pixel_size", pointillism_pixel_size);
        set_mixed_float("mixed_filament_pointillism_line_gap", pointillism_line_gap);
        set_mixed_float("mixed_filament_surface_indentation", mixed_surface_indentation);
        set_mixed_string("mixed_filament_definitions", mixed_mgr.serialize_custom_entries());
    }

    auto &mixed = mixed_mgr.mixed_filaments();
    const std::vector<size_t> ordered_mixed_indices = build_mixed_filament_ui_indices(mixed, p->m_mixed_filament_ui_order);
    std::vector<uint64_t>       sanitized_mixed_ui_order_ids;
    sanitized_mixed_ui_order_ids.reserve(ordered_mixed_indices.size());
    for (const size_t mixed_id : ordered_mixed_indices) {
        if (mixed_id < mixed.size() && mixed[mixed_id].stable_id != 0)
            sanitized_mixed_ui_order_ids.emplace_back(mixed[mixed_id].stable_id);
    }
    p->m_mixed_filament_ui_order = std::move(sanitized_mixed_ui_order_ids);

    p->m_mixed_filament_drag_active = false;
    p->m_mixed_filament_drag_source_mixed_id = size_t(-1);
    p->m_mixed_filament_row_bindings.clear();

    const int compact_gap_x   = FromDIP(6);
    const int compact_gap_y   = FromDIP(4);
    const int compact_row_pad = FromDIP(6);
    const bool is_dark = wxGetApp().dark_mode();
    const wxColour mixed_rows_bg = is_dark ? wxColour(45, 45, 49) : wxColour(246, 248, 251);
    const wxColour mixed_row_bg = is_dark ? wxColour(52, 52, 56) : wxColour(255, 255, 255);
    const wxColour mixed_row_hover_bg = is_dark ? wxColour(62, 62, 68) : wxColour(241, 247, 255);
    const wxColour mixed_text_fg = is_dark ? wxColour(232, 232, 232) : wxColour(20, 20, 20);
    const wxColour mixed_summary_fg = is_dark ? wxColour(182, 182, 182) : wxColour(96, 96, 96);
    p->m_panel_mixed_filaments_content->SetBackgroundColour(mixed_rows_bg);

    // Get the content sizer and clear it
    wxSizer *content_sizer = p->m_panel_mixed_filaments_content->GetSizer();
    if (content_sizer)
        content_sizer->Clear(true);
    
    // Re-add the top margin spacer that was added in constructor but cleared above
    if (content_sizer)
        content_sizer->AddSpacer(FromDIP(SidebarProps::ContentMargin()));

    // Update button states (buttons are now in title bar, created in constructor)
    if (p->m_btn_add_gradient)
        p->m_btn_add_gradient->Enable(num_physical >= 2);
    if (p->m_btn_add_pattern)
        p->m_btn_add_pattern->Enable(num_physical >= 2);
    if (p->m_btn_add_color)
        p->m_btn_add_color->Enable(num_physical >= 2);

    // Mixed Filaments panel is hidden
    p->m_panel_mixed_filaments_title->Hide();
    p->m_panel_mixed_filaments_content->Hide();
    Layout();
    refresh_model_canvas_colors();
    wxWeakRef<Sidebar> weakSelf(this);
    wxTheApp->CallAfter([weakSelf]() {
        if (weakSelf) weakSelf->update_color_mix_panel();
    });
    return;

#if 0 // DEAD CODE — Mixed Filaments panel UI, disabled. DO NOT add or modify code here.
      //    All active Mixed Filament operations now live in:
      //      - init_color_mix_panel()  (Color Mixing panel with Add/Delete buttons)
      //      - Filament Management title bar (Batch Match, Flushing volumes, AMS sync, Settings)
    
    // Reset the max size in case it was collapsed
    p->m_panel_mixed_filaments_content->SetMaxSize({-1, -1});

    auto *rows_scroller = new wxScrolledWindow(p->m_panel_mixed_filaments_content, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxTAB_TRAVERSAL);
    rows_scroller->SetScrollRate(0, FromDIP(6));
    rows_scroller->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);
    rows_scroller->SetBackgroundColour(mixed_rows_bg);
    auto *rows_sizer = new wxBoxSizer(wxVERTICAL);
    rows_scroller->SetSizer(rows_sizer);

    if (mixed.empty()) {
        auto *empty_label = new wxStaticText(rows_scroller, wxID_ANY,
                                             _L("No mixed filaments yet. Use Add Gradient, Add Pattern, or Add Color to create one."));
        empty_label->SetForegroundColour(mixed_summary_fg);
        empty_label->SetFont(::Label::Body_13);
        empty_label->Wrap(FromDIP(360));
        rows_sizer->Add(empty_label, 0, wxALL | wxEXPAND, FromDIP(12));
        rows_scroller->Layout();
        rows_scroller->FitInside();
        const int empty_content_h = empty_label->GetBestSize().GetHeight() + FromDIP(28);
        const int empty_rows_h = std::max(FromDIP(86), empty_content_h);
        rows_scroller->SetMinSize(wxSize(-1, empty_rows_h));
        rows_scroller->SetMaxSize(wxSize(-1, empty_rows_h));
        if (content_sizer)
            content_sizer->Add(rows_scroller, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(SidebarProps::ContentMargin()));
        p->m_panel_mixed_filaments_content->Layout();
        Layout();
        refresh_model_canvas_colors();
        update_color_mix_panel();
        return;
    }

    auto adjust_rows_scroller_height = [this, rows_scroller]() {
        if (!rows_scroller)
            return;
        const int min_h = FromDIP(68);
        const int collapsed_max_h = FromDIP(220);
        int two_rows_cap_h = collapsed_max_h;
        const auto &children = rows_scroller->GetChildren();
        if (!children.empty()) {
            std::vector<int> heights;
            heights.reserve(children.GetCount());
            for (wxWindowList::compatibility_iterator it = children.GetFirst(); it; it = it->GetNext()) {
                wxWindow *child = it->GetData();
                wxPanel *panel = dynamic_cast<wxPanel *>(child);
                if (!panel)
                    continue;
                heights.emplace_back(std::max(panel->GetSize().GetHeight(), panel->GetBestSize().GetHeight()));
            }
            if (!heights.empty()) {
                std::sort(heights.begin(), heights.end(), std::greater<int>());
                const size_t keep = std::min<size_t>(2, heights.size());
                int rows_h = 0;
                for (size_t i = 0; i < keep; ++i)
                    rows_h += heights[i];
                if (keep > 1)
                    rows_h += int(keep - 1) * FromDIP(2);
                rows_h += FromDIP(8);
                two_rows_cap_h = std::max(collapsed_max_h, rows_h);
            }
        }

        const int max_h = p->m_expanded_mixed_filament_rows.empty() ? collapsed_max_h : two_rows_cap_h;
        const int content_h = std::max(0, rows_scroller->GetVirtualSize().GetHeight());
        const int desired_h = std::clamp(content_h, min_h, max_h);
        rows_scroller->SetMinSize(wxSize(-1, desired_h));
        rows_scroller->SetMaxSize(wxSize(-1, desired_h));
    };

    for (auto it = p->m_expanded_mixed_filament_rows.begin(); it != p->m_expanded_mixed_filament_rows.end();) {
        if (*it >= mixed.size() || mixed[*it].deleted)
            it = p->m_expanded_mixed_filament_rows.erase(it);
        else
            ++it;
    }

    std::vector<wxColour> palette;
    palette.reserve(physical_colors.size());
    for (const std::string &hex : physical_colors)
        palette.emplace_back(parse_mixed_color(hex));

    auto mixed_summary_text = [decode_gradient_ids](const MixedFilament &entry) {
        const std::string normalized_pattern = MixedFilamentManager::normalize_manual_pattern(entry.manual_pattern);
        if (!entry.custom)
            return wxString::Format("(Filament %u + Filament %u)", unsigned(entry.component_a), unsigned(entry.component_b));
        if (!normalized_pattern.empty())
            return _L("(Pattern)");
        if (decode_gradient_ids(entry.gradient_component_ids).size() >= 3)
            return _L("(Color)");
        return wxString::Format("(F%u + F%u)", unsigned(entry.component_a), unsigned(entry.component_b));
    };

    auto apply_mixed_entry_changes = [this, preset_bundle, print_cfg, num_physical](size_t mixed_id,
                                                                                    const MixedFilament &updated_mf,
                                                                                    bool preserve_enabled = false,
                                                                                    bool rebuild_virtual_id_remap = false) {
        if (!preset_bundle)
            return;

        auto &mgr = preset_bundle->mixed_filaments;
        auto &mfs = mgr.mixed_filaments();
        if (mixed_id >= mfs.size())
            return;

        const std::vector<MixedFilament> old_mixed = rebuild_virtual_id_remap ? mfs : std::vector<MixedFilament>();
        MixedFilament merged = updated_mf;
        if (preserve_enabled)
            merged.enabled = mfs[mixed_id].enabled;
        mfs[mixed_id] = merged;

        const std::string serialized = mgr.serialize_custom_entries();
        if (print_cfg) {
            if (ConfigOptionString *opt = print_cfg->option<ConfigOptionString>("mixed_filament_definitions"))
                opt->value = serialized;
            else
                print_cfg->set_key_value("mixed_filament_definitions", new ConfigOptionString(serialized));
        }
        if (ConfigOptionString *opt = preset_bundle->project_config.option<ConfigOptionString>("mixed_filament_definitions"))
            opt->value = serialized;
        else
            preset_bundle->project_config.set_key_value("mixed_filament_definitions", new ConfigOptionString(serialized));

        if (print_cfg) {
            if (auto *print_tab = wxGetApp().get_tab(Preset::TYPE_PRINT))
                print_tab->update_dirty();
            if (wxGetApp().mainframe)
                wxGetApp().mainframe->on_config_changed(print_cfg);
        }
        if (wxGetApp().plater())
            wxGetApp().plater()->update_project_dirty_from_presets();

        if (rebuild_virtual_id_remap)
            preset_bundle->update_mixed_filament_id_remap(old_mixed, num_physical, num_physical);

        int mode = 0;
        if (const ConfigOptionBool *opt = preset_bundle->project_config.option<ConfigOptionBool>("mixed_filament_gradient_mode"))
            mode = opt->value ? 1 : 0;
        else if (const ConfigOptionInt *opt = preset_bundle->project_config.option<ConfigOptionInt>("mixed_filament_gradient_mode"))
            mode = opt->value != 0 ? 1 : 0;
        float lo = preset_bundle->project_config.has("mixed_filament_height_lower_bound") ?
            float(preset_bundle->project_config.opt_float("mixed_filament_height_lower_bound")) : 0.04f;
        float hi = preset_bundle->project_config.has("mixed_filament_height_upper_bound") ?
            float(preset_bundle->project_config.opt_float("mixed_filament_height_upper_bound")) : 0.16f;
        bool advanced = false;
        if (const ConfigOptionBool *opt = preset_bundle->project_config.option<ConfigOptionBool>("mixed_filament_advanced_dithering"))
            advanced = opt->value;
        mode = std::clamp(mode, 0, 1);
        lo = std::max(0.01f, lo);
        hi = std::max(lo, hi);
        mgr.apply_gradient_settings(mode, lo, hi, advanced);
        update_dynamic_filament_list();

        if (rebuild_virtual_id_remap && wxGetApp().plater()) {
            p->m_skip_mixed_filament_sync_once = true;
            wxGetApp().plater()->on_filaments_change(num_physical);
        }
    };

    auto current_mixed_filament_ui_order = [this, &mixed]() {
        std::vector<uint64_t> ordered_ids;
        ordered_ids.reserve(p->m_mixed_filament_row_bindings.size());
        for (const auto &binding : p->m_mixed_filament_row_bindings) {
            if (binding.mixed_id < mixed.size() && mixed[binding.mixed_id].stable_id != 0)
                ordered_ids.emplace_back(mixed[binding.mixed_id].stable_id);
        }
        return ordered_ids;
    };

    auto drop_insert_position = [this]() {
        const wxPoint mouse_pos = wxGetMousePosition();
        size_t        visible_idx = 0;
        for (const auto &binding : p->m_mixed_filament_row_bindings) {
            if (binding.row == nullptr || !binding.row->IsShown())
                continue;

            const wxPoint top_left = binding.row->ClientToScreen(wxPoint(0, 0));
            const int     row_h = std::max(binding.row->GetSize().GetHeight(), binding.row->GetBestSize().GetHeight());
            const int     center_y = top_left.y + row_h / 2;
            if (mouse_pos.y < center_y)
                return visible_idx;

            ++visible_idx;
        }
        return visible_idx;
    };

    for (size_t display_mixed_idx = 0; display_mixed_idx < ordered_mixed_indices.size(); ++display_mixed_idx) {
        const size_t mixed_id = ordered_mixed_indices[display_mixed_idx];
        MixedFilament &mf = mixed[mixed_id];
        const bool auto_row = !mf.custom;

        auto *row = new wxPanel(rows_scroller, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
        row->SetBackgroundColour(mixed_row_bg);
        auto *row_sizer = new wxBoxSizer(wxVERTICAL);
        p->m_mixed_filament_row_bindings.push_back({mixed_id, row});

        auto *header_panel = new wxPanel(row, wxID_ANY);
        header_panel->SetBackgroundColour(mixed_row_bg);
        auto *header_sizer = new wxBoxSizer(wxHORIZONTAL);

        const std::string synced_color = compute_entry_display_color(mf);
        if (mf.display_color != synced_color)
            mf.display_color = synced_color;
        auto *drag_handle = new MixedFilamentDragHandle(header_panel, mixed_summary_fg, mixed_row_bg);
        drag_handle->SetToolTip(_L("Drag to reorder mixed filaments in this panel."));
        header_sizer->Add(drag_handle, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, compact_gap_x);

        wxColour swatch_color = parse_mixed_color(mf.display_color);
        auto *swatch = new wxPanel(header_panel, wxID_ANY, wxDefaultPosition, wxSize(FromDIP(12), FromDIP(12)));
        swatch->SetBackgroundColour(swatch_color);
        swatch->SetMinSize(wxSize(FromDIP(12), FromDIP(12)));
        header_sizer->Add(swatch, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, compact_gap_x);

        const int virtual_filament_id = int(num_physical + display_mixed_idx + 1);
        auto *name_label = new wxStaticText(header_panel, wxID_ANY, wxString::Format("Mixed Filament %d", virtual_filament_id));
        name_label->SetForegroundColour(mixed_text_fg);
        header_sizer->Add(name_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, compact_gap_x);

        auto *summary_label = new wxStaticText(header_panel, wxID_ANY, mixed_summary_text(mf));
        summary_label->SetForegroundColour(mixed_summary_fg);
        header_sizer->Add(summary_label, 1, wxALIGN_CENTER_VERTICAL | wxLEFT, compact_gap_x);

        header_sizer->AddStretchSpacer(1);

        auto *enabled_chk = new wxCheckBox(header_panel, wxID_ANY, _L("Enabled"));
        enabled_chk->SetValue(mf.enabled);
        enabled_chk->SetForegroundColour(mixed_text_fg);
        header_sizer->Add(enabled_chk, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, compact_gap_x);
        enabled_chk->Bind(wxEVT_LEFT_UP, [](wxMouseEvent &evt) {
            evt.StopPropagation();
            evt.Skip();
        });
        enabled_chk->Bind(wxEVT_CHECKBOX, [mixed_id, enabled_chk, apply_mixed_entry_changes, preset_bundle](wxCommandEvent &) {
            if (!preset_bundle || !enabled_chk)
                return;
            auto &mgr = preset_bundle->mixed_filaments;
            auto &mfs = mgr.mixed_filaments();
            if (mixed_id >= mfs.size())
                return;
            MixedFilament updated = mfs[mixed_id];
            updated.enabled = enabled_chk->GetValue();
            apply_mixed_entry_changes(mixed_id, updated, false, true);
        });

        auto *del_btn = new ScalableButton(header_panel, wxID_ANY, "cross"); 
        del_btn->SetToolTip(_L("Delete mixed filament"));
        header_sizer->Add(del_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, compact_gap_x);
        
        del_btn->Bind(wxEVT_BUTTON, [this, mixed_id, num_physical, set_mixed_string, notify_mixed_change](wxCommandEvent&) {
             if (wxGetApp().preset_bundle) {
                 auto &mgr = wxGetApp().preset_bundle->mixed_filaments;
                 auto &mfs = mgr.mixed_filaments();
                 if (mixed_id < mfs.size()) {
                     const std::vector<MixedFilament> old_mixed = mfs;
                     auto canonical_pair = [](unsigned int a, unsigned int b) {
                         return std::make_pair(std::min(a, b), std::max(a, b));
                     };
                     MixedFilament &target = mfs[mixed_id];
                     const auto target_pair = canonical_pair(target.component_a, target.component_b);
                     const bool valid_auto_pair = target_pair.first >= 1 &&
                                                  target_pair.second >= 1 &&
                                                  target_pair.first <= num_physical &&
                                                  target_pair.second <= num_physical &&
                                                  target_pair.first != target_pair.second;
                     if (target.custom && target.origin_auto && valid_auto_pair) {
                         bool tombstoned_existing_auto = false;
                         for (size_t idx = 0; idx < mfs.size(); ++idx) {
                             if (idx == mixed_id)
                                 continue;
                             MixedFilament &candidate = mfs[idx];
                             if (candidate.custom)
                                 continue;
                             if (canonical_pair(candidate.component_a, candidate.component_b) != target_pair)
                                 continue;
                             candidate.deleted = true;
                             candidate.enabled = false;
                             tombstoned_existing_auto = true;
                             break;
                         }

                         if (tombstoned_existing_auto) {
                             mfs.erase(mfs.begin() + mixed_id);
                         } else {
                             target.component_a = target_pair.first;
                             target.component_b = target_pair.second;
                             target.mix_b_percent = 50;
                             target.ratio_a = 1;
                             target.ratio_b = 1;
                             target.manual_pattern.clear();
                             target.gradient_component_ids.clear();
                             target.gradient_component_weights.clear();
                             target.pointillism_all_filaments = false;
                             target.distribution_mode = int(MixedFilament::Simple);
                             target.custom = false;
                             target.origin_auto = true;
                             target.deleted = true;
                             target.enabled = false;
                         }
                     } else if (target.custom) {
                         mfs.erase(mfs.begin() + mixed_id);
                     } else {
                         target.deleted = true;
                         target.enabled = false;
                     }
                     p->m_expanded_mixed_filament_rows.clear();
                     set_mixed_string("mixed_filament_definitions", mgr.serialize_custom_entries());
                     wxGetApp().preset_bundle->update_mixed_filament_id_remap(old_mixed, num_physical, num_physical, mixed_id);
                     notify_mixed_change();
                     if (wxGetApp().plater())
                         wxGetApp().plater()->update_project_dirty_from_presets();
                     if (wxGetApp().plater()) {
                         p->m_skip_mixed_filament_sync_once = true;
                         wxGetApp().plater()->on_filaments_change(num_physical);
                     }
                 }
             }
        });

        header_panel->SetSizer(header_sizer);
        row_sizer->Add(header_panel, 0, wxEXPAND | wxALL, 0);

        auto *editor_host = new wxPanel(row, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
        editor_host->SetBackgroundColour(mixed_row_bg);
        auto *editor_sizer = new wxBoxSizer(wxVERTICAL);
        editor_host->SetSizer(editor_sizer);
        editor_host->Hide();
        row_sizer->Add(editor_host, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, compact_row_pad);

        auto set_row_hover = [row, header_panel, editor_host, drag_handle, mixed_summary_fg, mixed_row_bg, mixed_row_hover_bg](bool hovered) {
            const wxColour bg = hovered ? mixed_row_hover_bg : mixed_row_bg;
            if (row) row->SetBackgroundColour(bg);
            if (header_panel) header_panel->SetBackgroundColour(bg);
            if (editor_host) editor_host->SetBackgroundColour(bg);
            if (drag_handle) drag_handle->set_colors(mixed_summary_fg, bg);
            if (row) row->Refresh();
            if (header_panel) header_panel->Refresh();
            if (editor_host) editor_host->Refresh();
        };

        auto row_contains_mouse = [row]() {
            if (!row)
                return false;
            const wxPoint mouse_pos = wxGetMousePosition();
            const wxPoint local = row->ScreenToClient(mouse_pos);
            return row->GetClientRect().Contains(local);
        };

        auto ensure_editor = [this, mixed_id, num_physical, physical_colors, nozzle_diameters, palette, preview_settings, component_bias_enabled, preset_bundle,
                              editor_host, editor_sizer, swatch, summary_label, header_panel, row,
                              rows_scroller, mixed_summary_text, apply_mixed_entry_changes]() {
            if (!preset_bundle || !editor_sizer || editor_sizer->GetItemCount() > 0)
                return;

            auto &mgr = preset_bundle->mixed_filaments;
            auto &mfs = mgr.mixed_filaments();
            if (mixed_id >= mfs.size())
                return;

            auto *editor = new MixedFilamentConfigPanel(editor_host, mixed_id, mfs[mixed_id], num_physical, physical_colors, nozzle_diameters, palette, preview_settings,
                component_bias_enabled,
                [this, mixed_id, swatch, summary_label, header_panel, row, rows_scroller, mixed_summary_text, apply_mixed_entry_changes](const MixedFilament &updated_mf) {
                    apply_mixed_entry_changes(mixed_id, updated_mf, true);

                    if (swatch) {
                        swatch->SetBackgroundColour(parse_mixed_color(updated_mf.display_color));
                        swatch->Refresh();
                    }
                    if (summary_label) {
                        summary_label->SetLabel(mixed_summary_text(updated_mf));
                    }
                    if (header_panel)
                        header_panel->Layout();
                    if (row)
                        row->Layout();
                    if (rows_scroller) {
                        rows_scroller->Layout();
                        rows_scroller->FitInside();
                    }
                });

            editor_sizer->Add(editor, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(4));
            editor_host->Layout();
        };

        auto toggle_editor = [this, mixed_id, editor_host, ensure_editor, rows_scroller, adjust_rows_scroller_height]() {
            if (!editor_host || !rows_scroller)
                return;

            if (editor_host->IsShown()) {
                editor_host->Hide();
                p->m_expanded_mixed_filament_rows.erase(mixed_id);
            } else {
                ensure_editor();
                editor_host->Show();
                p->m_expanded_mixed_filament_rows.insert(mixed_id);
            }

            rows_scroller->Layout();
            rows_scroller->FitInside();
            adjust_rows_scroller_height();
            p->m_panel_mixed_filaments_content->Layout();
            m_scrolled_sizer->Layout();
            Layout();
        };

        auto bind_toggle_target = [&toggle_editor](wxWindow *target) {
            if (!target)
                return;
            target->SetCursor(wxCursor(wxCURSOR_HAND));
            target->Bind(wxEVT_LEFT_UP, [toggle_editor](wxMouseEvent &) {
                toggle_editor();
            });
        };

        auto bind_hover_target = [set_row_hover, row_contains_mouse](wxWindow *target) {
            if (!target)
                return;
            target->Bind(wxEVT_ENTER_WINDOW, [set_row_hover](wxMouseEvent &evt) {
                set_row_hover(true);
                evt.Skip();
            });
            target->Bind(wxEVT_LEAVE_WINDOW, [set_row_hover, row_contains_mouse](wxMouseEvent &evt) {
                set_row_hover(row_contains_mouse());
                evt.Skip();
            });
        };

        auto release_drag_capture = [this]() {
            p->m_mixed_filament_drag_active = false;
            p->m_mixed_filament_drag_source_mixed_id = size_t(-1);
        };

        auto bind_drag_target = [this,
                                 mixed_id,
                                 &mixed,
                                 drop_insert_position,
                                 current_mixed_filament_ui_order,
                                 release_drag_capture](wxWindow *target) {
            if (!target)
                return;

            target->Bind(wxEVT_LEFT_DOWN, [this, mixed_id, target](wxMouseEvent &evt) {
                if (!target)
                    return;
                p->m_mixed_filament_drag_active = true;
                p->m_mixed_filament_drag_source_mixed_id = mixed_id;
                if (!target->HasCapture())
                    target->Capt�my��$z{-���jם();
    sidebar_layout.show = new_sel == MainFrame::tp3DEditor || new_sel == MainFrame::tpPreview;
    update_sidebar();
    int old_sel = e.GetOldSelection();
    if (wxGetApp().preset_bundle && wxGetApp().preset_bundle->use_bbl_device_tab() && new_sel == MainFrame::tpMonitor &&
        wxGetApp().app_config->get("use_new_connect") != "true") {
        if (!wxGetApp().getAgent()) {
            e.Veto();
            BOOST_LOG_TRIVIAL(info) << boost::format("skipped tab switch from %1% to %2%, lack of network plugins") % old_sel % new_sel;
            if (q) {
                wxCommandEvent* evt = new wxCommandEvent(EVT_INSTALL_PLUGIN_HINT);
                wxQueueEvent(q, evt);
            }
        }
    } else {
        if (new_sel == MainFrame::tpMonitor && wxGetApp().preset_bundle != nullptr) {
            auto     cfg = wxGetApp().preset_bundle->printers.get_edited_preset().config;
            wxString url = cfg.opt_string("print_host_webui").empty() ? cfg.opt_string("print_host") : cfg.opt_string("print_host_webui");
            if (main_frame->m_printer_view && url.empty()) {
                // It's missing_connection page, reload so that we can replay the gif image
                // main_frame->m_printer_view->reload();
            }
        }
    }
}

int Plater::priv::update_print_required_data(Slic3r::DynamicPrintConfig config, Slic3r::Model model, Slic3r::PlateDataPtrs plate_data_list, std::string file_name, std::string file_path)
{
    if (!m_select_machine_dlg) m_select_machine_dlg = new SelectMachineDialog(q);
    return m_select_machine_dlg->update_print_required_data(config, model, plate_data_list, file_name, file_path);
}

void Plater::priv::on_action_send_to_printer(bool isall)
{
	if (!m_send_to_sdcard_dlg) m_send_to_sdcard_dlg = new SendToPrinterDialog(q);
    if (isall) {
        m_send_to_sdcard_dlg->prepare(PLATE_ALL_IDX);
    }
    else {
        m_send_to_sdcard_dlg->prepare(partplate_list.get_curr_plate_index());
    }

	m_send_to_sdcard_dlg->ShowModal();
}


void Plater::priv::on_action_select_sliced_plate(wxCommandEvent &evt)
{
    if (q != nullptr) {
        BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << ":received select sliced plate event\n" ;
    }
    q->select_sliced_plate(evt.GetInt());
}

void Plater::priv::on_action_print_all(SimpleEvent&)
{
    if (q != nullptr) {
        BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << ":received print all event\n" ;
    }

    PresetBundle& preset_bundle = *wxGetApp().preset_bundle;
    if (preset_bundle.use_bbl_network()) {
        // BBS
        if (!m_select_machine_dlg)
            m_select_machine_dlg = new SelectMachineDialog(q);
        m_select_machine_dlg->set_print_type(PrintFromType::FROM_NORMAL);
        m_select_machine_dlg->prepare(PLATE_ALL_IDX);
        m_select_machine_dlg->ShowModal();
        record_start_print_preset("print_all");
    } else {
        q->send_gcode_legacy(PLATE_ALL_IDX, nullptr, true);
    }
}

void Plater::priv::on_action_export_gcode(SimpleEvent&)
{
    if (q != nullptr) {
        BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << ":received export gcode event\n" ;
        q->export_gcode(false);
    }
}

void Plater::priv::on_action_send_gcode(SimpleEvent&)
{
    if (q != nullptr) {
        BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << ":received export gcode event\n" ;
        q->send_gcode_legacy();
    }
}

void Plater::priv::on_action_export_sliced_file(SimpleEvent&)
{
    if (q != nullptr) {
        BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << ":received export sliced file event\n" ;
        q->export_gcode_3mf();
    }
}

void Plater::priv::on_action_export_all_sliced_file(SimpleEvent &)
{
    if (q != nullptr) {
        BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << ":received export all sliced file event\n";
        q->export_gcode_3mf(true);
    }
}

void Plater::priv::on_action_export_to_sdcard(SimpleEvent&)
{
	if (q != nullptr) {
		BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << ":received export sliced file event\n";
		q->send_to_printer();
	}
}

void Plater::priv::on_action_export_to_sdcard_all(SimpleEvent&)
{
    if (q != nullptr) {
        BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << ":received export sliced file event\n";
        q->send_to_printer(true);
    }
}

//BBS: add plate select logic
void Plater::priv::on_plate_selected(SimpleEvent&)
{
    BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << ":received plate selected event\n" ;
    sidebar->obj_list()->on_plate_selected(partplate_list.get_curr_plate_index());
}

void Plater::priv::on_action_request_model_id(wxCommandEvent& evt)
{
    BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << ":received import model id event\n" ;
    if (q != nullptr) {
        q->import_model_id(evt.GetString());
    }
}

void Plater::priv::on_action_download_project(wxCommandEvent& evt)
{
    BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << ":received download project event\n" ;
    if (q != nullptr) {
        q->download_project(evt.GetString());
    }
}

//BBS: add slice button status update logic
void Plater::priv::on_slice_button_status(bool enable)
{
    BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << ": enable = "<<enable<<"\n";
    if (!background_process.running())
        main_frame->update_slice_print_status(MainFrame::eEventObjectUpdate, enable);
}

void Plater::priv::on_action_split_objects(SimpleEvent&)
{
    split_object();
}

void Plater::priv::on_action_split_volumes(SimpleEvent&)
{
    split_volume();
}

void Plater::priv::on_object_select(SimpleEvent& evt)
{
    wxGetApp().obj_list()->update_selections();
    selection_changed();
}

//BBS: repair model through netfabb
void Plater::priv::on_repair_model(wxCommandEvent &event)
{
    wxGetApp().obj_list()->fix_through_netfabb();
}

bool Plater::priv::confirm_auto_generated_gradients(wxWindow *parent, size_t num_physical)
{
    auto *app_config = wxGetApp().app_config;
    if (app_config == nullptr)
        return MixedFilamentManager::auto_generate_enabled();

    const bool pref_enabled = app_config->get_bool("auto_generate_gradients");
    if (!pref_enabled) {
        m_last_auto_gradient_prompt_physical_count = 0;
        m_last_auto_gradient_prompt_accepted = false;
        MixedFilamentManager::set_auto_generate_enabled(false);
        return false;
    }

    if (num_physical <= 4) {
        m_last_auto_gradient_prompt_physical_count = 0;
        m_last_auto_gradient_prompt_accepted = false;
        MixedFilamentManager::set_auto_generate_enabled(true);
        return true;
    }

    if (parent == nullptr || !parent->IsShownOnScreen()) {
        m_last_auto_gradient_prompt_physical_count = 0;
        m_last_auto_gradient_prompt_accepted = false;
        MixedFilamentManager::set_auto_generate_enabled(true);
        return true;
    }

    if (m_last_auto_gradient_prompt_physical_count == num_physical) {
        MixedFilamentManager::set_auto_generate_enabled(m_last_auto_gradient_prompt_accepted);
        return m_last_auto_gradient_prompt_accepted;
    }

    const size_t auto_gradient_count = num_physical * (num_physical - 1) / 2;
    const wxString message = wxString::Format(
        _L("Using %d physical filaments will create %d auto-generated gradients.\nDo you want to create them now?"),
        int(num_physical),
        int(auto_gradient_count));
    const int result = MessageDialog(parent,
                                     message,
                                     wxString(SLIC3R_APP_FULL_NAME) + " - " + _L("Auto gradients"),
                                     wxYES_NO | wxYES_DEFAULT | wxCENTRE | wxICON_QUESTION)
                           .ShowModal();
    const bool accepted = result == wxID_YES;
    m_last_auto_gradient_prompt_physical_count = num_physical;
    m_last_auto_gradient_prompt_accepted = accepted;
    MixedFilamentManager::set_auto_generate_enabled(accepted);
    return accepted;
}

void Plater::priv::set_auto_generated_gradient_decision(size_t num_physical, bool create_auto_gradients)
{
    m_last_auto_gradient_prompt_physical_count = num_physical;
    m_last_auto_gradient_prompt_accepted = create_auto_gradients;
    MixedFilamentManager::set_auto_generate_enabled(create_auto_gradients);
}

bool Plater::confirm_auto_generated_gradients(size_t num_physical)
{
    return p != nullptr ? p->confirm_auto_generated_gradients(this, num_physical) : MixedFilamentManager::auto_generate_enabled();
}

void Plater::set_auto_generated_gradient_decision(size_t num_physical, bool create_auto_gradients)
{
    if (p != nullptr)
        p->set_auto_generated_gradient_decision(num_physical, create_auto_gradients);
    else
        MixedFilamentManager::set_auto_generate_enabled(create_auto_gradients);
}

void Plater::priv::on_filament_color_changed(wxCommandEvent &event)
{
    //q->update_all_plate_thumbnails(true);
    //q->get_preview_canvas3D()->update_plate_thumbnails();
    int modify_id = event.GetInt();

    auto& ams_multi_color_filment = wxGetApp().preset_bundle->ams_multi_color_filment;
    if (modify_id >= 0 && modify_id < ams_multi_color_filment.size())
        ams_multi_color_filment[modify_id].clear();

    if (wxGetApp().app_config->get("auto_calculate") == "true") {
        sidebar->auto_calc_flushing_volumes(modify_id);
    }

    // Regenerate mixed filaments and refresh the mixed panel only. Color
    // changes do not alter filament IDs, so the full on_filaments_change()
    // path is unnecessary and can re-enter UI rebuilds mid-update.
    wxGetApp().preset_bundle->update_multi_material_filament_presets();
    sidebar->update_mixed_filament_panel();
    sidebar->update_color_mix_panel();
}

void Plater::priv::install_network_plugin(wxCommandEvent &event)
{
    wxGetApp().ShowDownNetPluginDlg();
    return;
}

void Plater::priv::update_plugin_when_launch(wxCommandEvent &event)
{
    std::string data_dir_str = data_dir();
    boost::filesystem::path data_dir_path(data_dir_str);
    auto cache_folder = data_dir_path / "ota";
    std::string changelog_file = cache_folder.string() + "/network_plugins.json";

    UpdatePluginDialog dlg(wxGetApp().mainframe);
    dlg.update_info(changelog_file);
    auto result = dlg.ShowModal();

    auto app_config = wxGetApp().app_config;
    if (!app_config) return;

    if (result == wxID_OK) {
        app_config->set("update_network_plugin", "true");
    }
    else if (result == wxID_NO) {
        app_config->set("update_network_plugin", "false");
    }
}

void Plater::priv::show_install_plugin_hint(wxCommandEvent &event)
{
    notification_manager->bbl_show_plugin_install_notification(into_u8(_L("Network Plug-in is not detected. Network related features are unavailable.")));
}

void Plater::priv::show_preview_only_hint(wxCommandEvent &event)
{
    notification_manager->bbl_show_preview_only_notification(into_u8(_L("Preview only mode:\nThe loaded file contains G-code only, cannot enter the Prepare page.")));
}

void Plater::priv::on_apple_change_color_mode(wxSysColourChangedEvent& evt) {
    m_is_dark = wxSystemSettings::GetAppearance().IsDark();
    if (view3D->get_canvas3d() && view3D->get_canvas3d()->is_initialized()) {
        view3D->get_canvas3d()->on_change_color_mode(m_is_dark);
        preview->get_canvas3d()->on_change_color_mode(m_is_dark);
        assemble_view->get_canvas3d()->on_change_color_mode(m_is_dark);
    }

    apply_color_mode();
}

void Plater::priv::on_change_color_mode(SimpleEvent& evt) {
    m_is_dark = wxGetApp().app_config->get("dark_color_mode") == "1";
    view3D->get_canvas3d()->on_change_color_mode(m_is_dark);
    preview->get_canvas3d()->on_change_color_mode(m_is_dark);
    assemble_view->get_canvas3d()->on_change_color_mode(m_is_dark);
    if (m_send_to_sdcard_dlg) m_send_to_sdcard_dlg->on_change_color_mode();

    apply_color_mode();
}

void Plater::priv::apply_color_mode()
{
    const bool is_dark         = wxGetApp().dark_mode();
    wxColour   orca_color      = wxColour(59, 68, 70);//wxColour(ColorRGBA::ORCA().r_uchar(), ColorRGBA::ORCA().g_uchar(), ColorRGBA::ORCA().b_uchar());
    orca_color                 = is_dark ? StateColor::darkModeColorFor(orca_color) : StateColor::lightModeColorFor(orca_color);
    wxColour sash_color = is_dark ? wxColour(38, 46, 48) : wxColour(206, 206, 206);
    m_aui_mgr.GetArtProvider()->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, sash_color);
    m_aui_mgr.GetArtProvider()->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, *wxWHITE);
    m_aui_mgr.GetArtProvider()->SetColour(wxAUI_DOCKART_SASH_COLOUR, sash_color);
    m_aui_mgr.GetArtProvider()->SetColour(wxAUI_DOCKART_BORDER_COLOUR, is_dark ? *wxBLACK : wxColour(165, 165, 165));

    if (sidebar)
        sidebar->update_color_mix_panel();
}

static void get_position(wxWindowBase* child, wxWindowBase* until_parent, int& x, int& y) {
    int res_x = 0, res_y = 0;

    while (child != until_parent && child != nullptr) {
        int _x, _y;
        child->GetPosition(&_x, &_y);
        res_x += _x;
        res_y += _y;

        child = child->GetParent();
    }

    x = res_x;
    y = res_y;
}

void Plater::priv::show_right_click_menu(Vec2d mouse_position, wxMenu *menu)
{
    // BBS: GUI refactor: move sidebar to the left
    int x, y;
    get_position(current_panel, wxGetApp().mainframe, x, y);
    wxPoint position(static_cast<int>(mouse_position.x() + x), static_cast<int>(mouse_position.y() + y));
#ifdef __linux__
    // For some reason on Linux the menu isn't displayed if position is
    // specified (even though the position is sane).
    position = wxDefaultPosition;
#endif
    GLCanvas3D &canvas = *q->canvas3D();
    canvas.apply_retina_scale(mouse_position);
    canvas.set_popup_menu_position(mouse_position);
    q->PopupMenu(menu, position);
    canvas.clear_popup_menu_position();
}

void Plater::priv::on_right_click(RBtnEvent& evt)
{
    int obj_idx = get_selected_object_idx();

    wxMenu* menu = nullptr;

    if (obj_idx == -1) { // no one or several object are selected
        if (evt.data.second) { // right button was clicked on empty space
            if (!get_selection().is_empty()) // several objects are selected in 3DScene
                return;
            menu = menus.default_menu();
        }
        else {
            if (current_panel == assemble_view) {
                menu = menus.assemble_multi_selection_menu();
            }
            else {
                menu = menus.multi_selection_menu();
            }
        }
    }
    else {
        // If in 3DScene is(are) selected volume(s), but right button was clicked on empty space
        if (evt.data.second)
            return;

        // Each context menu respects to the selected item in ObjectList,
        // so this selection should be updated before menu agyuicreation
        wxGetApp().obj_list()->update_selections();

        if (printer_technology == ptSLA)
            menu = menus.sla_object_menu();
        else {
            const Selection& selection = get_selection();
            // show "Object menu" for each one or several FullInstance instead of FullObject
            const bool is_some_full_instances = selection.is_single_full_instance() ||
                                                selection.is_single_full_object() ||
                                                selection.is_multiple_full_instance();
            const bool is_part = selection.is_single_volume() || selection.is_single_modifier();

            //BBS get assemble view menu
            if (current_panel == assemble_view) {
                menu = is_some_full_instances   ? menus.assemble_object_menu() :
                   is_part                  ? menus.assemble_part_menu()   : menus.assemble_multi_selection_menu();
            } else {
                if (is_some_full_instances)
                    menu = printer_technology == ptSLA ? menus.sla_object_menu() : menus.object_menu();
                else if (is_part) {
                    const GLVolume* gl_volume = selection.get_first_volume();
                    const ModelVolume *model_volume = get_model_volume(*gl_volume, selection.get_model()->objects);
                    menu = (model_volume != nullptr && model_volume->is_text()) ? menus.text_part_menu() :
                           (model_volume != nullptr && model_volume->is_svg()) ? menus.svg_part_menu() : 
                        menus.part_menu();
                } else
                    menu = menus.multi_selection_menu();
            }
        }
    }

    if (q != nullptr && menu) {
        show_right_click_menu(evt.data.first, menu);
    }
}

//BBS: add part plate related logic
void Plater::priv::on_plate_right_click(RBtnPlateEvent& evt)
{
    wxMenu *menu = menus.plate_menu();
    show_right_click_menu(evt.data.first, menu);
}

void Plater::priv::on_update_geometry(Vec3dsEvent<2>&)
{
    // TODO
}

void Plater::priv::on_3dcanvas_mouse_dragging_started(SimpleEvent&)
{
    view3D->get_canvas3d()->reset_sequential_print_clearance();
}

// Update the scene from the background processing,
// if the update message was received during mouse manipulation.
void Plater::priv::on_3dcanvas_mouse_dragging_finished(SimpleEvent&)
{
    if (delayed_scene_refresh) {
        delayed_scene_refresh = false;
        update_sla_scene();
    }

    //partplate_list.reload_all_objects();
}

//BBS: add plate id for thumbnail generate param
void Plater::priv::generate_thumbnail(ThumbnailData& data, unsigned int w, unsigned int h, const ThumbnailsParams& thumbnail_params, Camera::EType camera_type, bool use_top_view, bool for_picking, bool ban_light)
{
    view3D->get_canvas3d()->render_thumbnail(data, w, h, thumbnail_params, camera_type, use_top_view, for_picking, ban_light);
}

//BBS: add plate id for thumbnail generate param
ThumbnailsList Plater::priv::generate_thumbnails(const ThumbnailsParams& params, Camera::EType camera_type)
{
    ThumbnailsList thumbnails;
    for (const Vec2d& size : params.sizes) {
        thumbnails.push_back(ThumbnailData());
        Point isize(size); // round to ints
        generate_thumbnail(thumbnails.back(), isize.x(), isize.y(), params, camera_type);
        if (!thumbnails.back().is_valid())
            thumbnails.pop_back();
    }
    return thumbnails;
}

void Plater::priv::generate_calibration_thumbnail(ThumbnailData& data, unsigned int w, unsigned int h, const ThumbnailsParams& thumbnail_params)
{
    preview->get_canvas3d()->render_calibration_thumbnail(data, w, h, thumbnail_params);
}

PlateBBoxData Plater::priv::generate_first_layer_bbox()
{
    PlateBBoxData bboxdata;
    std::vector<BBoxData>& id_bboxes = bboxdata.bbox_objs;
    BoundingBoxf bbox_all;
    auto                   print = this->background_process.m_fff_print;
    auto curr_plate = this->partplate_list.get_curr_plate();
    auto curr_plate_seq = curr_plate->get_real_print_seq();
    bboxdata.is_seq_print = (curr_plate_seq == PrintSequence::ByObject);
    bboxdata.first_extruder = print->get_tool_ordering().first_extruder();
    bboxdata.bed_type       = bed_type_to_gcode_string(print->config().curr_bed_type.value);
    // get nozzle diameter
    auto opt_nozzle_diameters = print->config().option<ConfigOptionFloats>("nozzle_diameter");
    if (opt_nozzle_diameters != nullptr)
        bboxdata.nozzle_diameter = float(opt_nozzle_diameters->get_at(bboxdata.first_extruder));
    //PrintObjectPtrs objects;
    //if (this->printer_technology == ptFFF) {
    //    objects = this->background_process.m_fff_print->objects().vector();
    //}
    //else {
    //    objects = this->background_process.m_sla_print->objects();
    //}
    auto objects = print->objects();
    auto orig = this->partplate_list.get_curr_plate()->get_origin();
    Vec2d orig2d = { orig[0], orig[1] };

    BBoxData data;
    for (auto obj : objects)
    {
        auto bb_scaled = obj->get_first_layer_bbox(data.area, data.layer_height, data.name);
        auto bb = unscaled(bb_scaled);
        bb.min -= orig2d;
        bb.max -= orig2d;
        bbox_all.merge(bb);
        data.area *= (SCALING_FACTOR * SCALING_FACTOR); // unscale area
        data.id = obj->id().id;
        data.bbox = { bb.min.x(),bb.min.y(),bb.max.x(),bb.max.y() };
        id_bboxes.emplace_back(data);
    }

    // add wipe tower bounding box
    if (print->has_wipe_tower()) {
        auto   wt_corners = print->first_layer_wipe_tower_corners();
        // when loading gcode.3mf, wipe tower info may not be correct
        if (!wt_corners.empty()) {
            BoundingBox bb_scaled = {wt_corners[0], wt_corners[2]};
            auto        bb        = unscaled(bb_scaled);
            bb.min -= orig2d;
            bb.max -= orig2d;
            bbox_all.merge(bb);
            data.name = "wipe_tower";
            data.id   = partplate_list.get_curr_plate()->get_index() + 1000;
            data.bbox = {bb.min.x(), bb.min.y(), bb.max.x(), bb.max.y()};
            id_bboxes.emplace_back(data);
        }
    }

    bboxdata.bbox_all = { bbox_all.min.x(),bbox_all.min.y(),bbox_all.max.x(),bbox_all.max.y() };
    return bboxdata;
}

wxString Plater::priv::get_project_filename(const wxString& extension) const
{
    if (m_project_name.empty())
        return "";
    else {
        auto full_filename = m_project_folder / std::string((m_project_name + extension).mb_str(wxConvUTF8));
        return m_project_folder.empty() ? "" : from_path(full_filename);
    }
}

wxString Plater::priv::get_export_gcode_filename(const wxString& extension, bool only_filename, bool export_all) const
{
    std::string plate_index_str;
    auto plate_name = partplate_list.get_curr_plate()->get_plate_name();
    if (!plate_name.empty())
        plate_index_str = (boost::format("_%1%") % plate_name).str();
    else if (partplate_list.get_plate_count() > 1)
        plate_index_str = (boost::format("_plate_%1%") % std::to_string(partplate_list.get_curr_plate_index() + 1)).str();

    if (!m_project_folder.empty()) {
        if (!only_filename) {
            if (export_all) {
                auto full_filename = m_project_folder / std::string((m_project_name + extension).mb_str(wxConvUTF8));
                return from_path(full_filename);
            } else {
                auto full_filename = m_project_folder / std::string((m_project_name + from_u8(plate_index_str) + extension).mb_str(wxConvUTF8));
                return from_path(full_filename);
            }
        } else {
            if (export_all)
                return m_project_name + extension;
            else
                return m_project_name + from_u8(plate_index_str) + extension;
        }
    } else {
        if (only_filename) {
            if(!model.objects.empty() && m_project_name == _L("Untitled"))
                return wxString(fs::path(model.objects.front()->name).replace_extension().c_str()) + from_u8(plate_index_str) + extension;

            if (export_all)
                return m_project_name + extension;
            else
                return m_project_name + from_u8(plate_index_str) + extension;
        }
        else
            return "";
    }
}

wxString Plater::priv::get_project_name()
{
    return m_project_name;
}

//BBS
void Plater::priv::set_project_name(const wxString& project_name)
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << __LINE__ << " project is:" << project_name;
    m_project_name = project_name;
    //update topbar title
#ifdef __WINDOWS__
    wxGetApp().mainframe->SetTitle(m_project_name + " - Snapmaker Orca");
    wxGetApp().mainframe->topbar()->SetTitle(m_project_name);
#else
    wxGetApp().mainframe->SetTitle(m_project_name);
    if (!m_project_name.IsEmpty())
        wxGetApp().mainframe->update_title_colour_after_set_title();
#endif
}

void Plater::priv::update_title_dirty_status()
{
    if (m_project_name.empty())
        return;

    wxString title;
    if (is_project_dirty())
        title = "*" + m_project_name;
    else
        title = m_project_name;

#ifdef __WINDOWS__
    wxGetApp().mainframe->topbar()->SetTitle(title);
#else
    wxGetApp().mainframe->SetTitle(title);
    wxGetApp().mainframe->update_title_colour_after_set_title();    
#endif    
}

void Plater::priv::set_project_filename(const wxString& filename)
{
    boost::filesystem::path full_path = into_path(filename);
    boost::filesystem::path ext = full_path.extension();
    //if (boost::iequals(ext.string(), ".amf")) {
    //    // Remove the first extension.
    //    full_path.replace_extension("");
    //    // It may be ".zip.amf".
    //    if (boost::iequals(full_path.extension().string(), ".zip"))
    //        // Remove the 2nd extension.
    //        full_path.replace_extension("");
    //} else {
    //    // Remove just one extension.
    //    full_path.replace_extension("");
    //}
    full_path.replace_extension("");

    m_project_folder = full_path.parent_path();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << __LINE__ << " project folder is:" << m_project_folder.string();

    //BBS
    wxString project_name = from_u8(full_path.filename().string());
    set_project_name(project_name);
    // record filename for hint when open exported file/.gcode
    if (q->m_only_gcode)
        q->m_preview_only_filename = std::string((project_name + ".gcode").mb_str());
    if (q->m_exported_file)
        q->m_preview_only_filename = std::string((project_name + ".3mf").mb_str());

    wxGetApp().mainframe->update_title();

    if (!m_project_folder.empty() && !q->m_only_gcode)
        wxGetApp().mainframe->add_to_recent_projects(filename);
}

void Plater::priv::init_notification_manager()
{
    if (!notification_manager)
        return;
    notification_manager->init();

    auto cancel_callback = [this]() {
        if (this->background_process.idle())
            return false;
        this->background_process.stop();
        return true;
    };
    notification_manager->init_slicing_progress_notification(cancel_callback);
    notification_manager->set_fff(printer_technology == ptFFF);
    notification_manager->init_progress_indicator();
}

void Plater::orient()
{
    auto &w = get_ui_job_worker();
    if (w.is_idle()) {
        p->take_snapshot(_u8L("Orient"));
        replace_job(w, std::make_unique<OrientJob>());
    }
}

//BBS: add job state related functions
void Plater::set_prepare_state(int state)
{
    p->m_job_prepare_state = state;
}

int Plater::get_prepare_state()
{
    return p->m_job_prepare_state;
}

void Plater::get_print_job_data(PrintPrepareData* data)
{
    if (data) {
        data->plate_idx = p->m_print_job_data.plate_idx;
        data->_3mf_path = p->m_print_job_data._3mf_path;
        data->_3mf_config_path = p->m_print_job_data._3mf_config_path;
    }
}

int Plater::get_send_calibration_finished_event()
{
    return EVT_SEND_CALIBRATION_FINISHED;
}

int Plater::get_print_finished_event()
{
    return EVT_PRINT_FINISHED;
}

int Plater::get_send_finished_event()
{
    return EVT_SEND_FINISHED;
}

int Plater::get_publish_finished_event()
{
    return EVT_PUBLISH_FINISHED;
}

void Plater::priv::set_current_canvas_as_dirty()
{
    if (current_panel == view3D)
        view3D->set_as_dirty();
    else if (current_panel == preview)
        preview->set_as_dirty();
    else if (current_panel == assemble_view)
        assemble_view->set_as_dirty();
}

GLCanvas3D* Plater::priv::get_current_canvas3D(bool exclude_preview)
{
    // During destruction, these pointers may be null or point to destroyed objects
    // Add null checks to prevent crashes during shutdown
    if (current_panel == view3D) {
        if (view3D)
            return view3D->get_canvas3d();
    }
    else if (!exclude_preview && (current_panel == preview)) {
        if (preview)
            return preview->get_canvas3d();
    }
    else if (current_panel == assemble_view) {
        if (assemble_view)
            return assemble_view->get_canvas3d();
    }
    
    //BBS default set to view3D, but check if it's still valid
    if (view3D)
        return view3D->get_canvas3d();
    
    return nullptr;
}

void Plater::priv::unbind_canvas_event_handlers()
{
    if (view3D != nullptr)
        view3D->get_canvas3d()->unbind_event_handlers();

    if (preview != nullptr)
        preview->get_canvas3d()->unbind_event_handlers();

    if (assemble_view != nullptr)
        assemble_view->get_canvas3d()->unbind_event_handlers();
}

void Plater::priv::reset_canvas_volumes()
{
    if (view3D != nullptr)
        view3D->get_canvas3d()->reset_volumes();

    if (preview != nullptr)
        preview->get_canvas3d()->reset_volumes();
}

bool Plater::priv::init_collapse_toolbar()
{
    if (wxGetApp().is_gcode_viewer())
        return true;

    if (collapse_toolbar.get_items_count() > 0)
        // already initialized
        return true;

    BackgroundTexture::Metadata background_data;
    background_data.filename = m_is_dark ? "toolbar_background_dark.png" : "toolbar_background.png";
    background_data.left = 16;
    background_data.top = 16;
    background_data.right = 16;
    background_data.bottom = 16;

    if (!collapse_toolbar.init(background_data))
        return false;

    collapse_toolbar.set_layout_type(GLToolbar::Layout::Vertical);
    collapse_toolbar.set_horizontal_orientation(GLToolbar::Layout::HO_Right);
    collapse_toolbar.set_vertical_orientation(GLToolbar::Layout::VO_Top);
    collapse_toolbar.set_border(4.0f);
    collapse_toolbar.set_separator_size(4);
    collapse_toolbar.set_gap_size(2);

    collapse_toolbar.del_all_item();

    GLToolbarItem::Data item;

    item.name = "collapse_sidebar";
    // set collapse svg name
    item.icon_filename = "collapse.svg";
    item.sprite_id = 0;
    item.left.action_callback = []() {
        wxGetApp().plater()->collapse_sidebar(!wxGetApp().plater()->is_sidebar_collapsed());
    };

    if (!collapse_toolbar.add_item(item))
        return false;

    // Now "collapse" sidebar to current state. This is done so the tooltip
    // is updated before the toolbar is first used.
    wxGetApp().plater()->collapse_sidebar(wxGetApp().plater()->is_sidebar_collapsed());
    return true;
}

void Plater::priv::update_preview_bottom_toolbar()
{
    ;
}

#if 0
void Plater::update_partplate()
{
    sidebar().update_partplate(p->partplate_list);
}
#endif

void Plater::priv::reset_gcode_toolpaths()
{
    preview->get_canvas3d()->reset_gcode_toolpaths();
}

bool Plater::priv::can_set_instance_to_object() const
{
    const int obj_idx = get_selected_object_idx();
    return 0 <= obj_idx && obj_idx < (int)model.objects.size() && model.objects[obj_idx]->instances.size() > 1;
}

bool Plater::priv::can_split(bool to_objects) const
{
    return sidebar->obj_list()->is_splittable(to_objects);
}

bool Plater::priv::can_fillcolor() const
{
    //BBS TODO
    return true;
}

bool Plater::priv::has_assemble_view() const
{
    for (auto object: model.objects)
    {
        for (auto instance : object->instances)
            if (instance->is_assemble_initialized())
                return true;

        int part_cnt = 0;
        for (auto volume : object->volumes) {
            if (volume->is_model_part())
                part_cnt++;
        }

        if (part_cnt > 1)
            return true;
    }
    return false;
}

#if ENABLE_ENHANCED_PRINT_VOLUME_FIT
bool Plater::priv::can_scale_to_print_volume() const
{
    const BuildVolume_Type type = this->bed.build_volume().type();
    return !sidebar->obj_list()->has_selected_cut_object()
        && !view3D->get_canvas3d()->get_selection().is_empty()
        && (type == BuildVolume_Type::Rectangle || type == BuildVolume_Type::Circle);
}
#endif // ENABLE_ENHANCED_PRINT_VOLUME_FIT

bool Plater::priv::can_mirror() const
{
    return !sidebar->obj_list()->has_selected_cut_object()
        && get_selection().is_from_single_instance();
}

bool Plater::priv::can_replace_with_stl() const
{
    return !sidebar->obj_list()->has_selected_cut_object()
        && get_selection().get_volume_idxs().size() == 1;
}

bool Plater::priv::can_reload_from_disk() const
{
    if (sidebar->obj_list()->has_selected_cut_object())
        return false;

#if ENABLE_RELOAD_FROM_DISK_REWORK
    // collect selected reloadable ModelVolumes
    std::vector<std::pair<int, int>> selected_volumes = reloadable_volumes(model, get_selection());
    // nothing to reload, return
    if (selected_volumes.empty())
        return false;
#else
    // struct to hold selected ModelVolumes by their indices
    struct SelectedVolume
    {
        int object_idx;
        int volume_idx;

        // operators needed by std::algorithms
        bool operator < (const SelectedVolume& other) const { return (object_idx < other.object_idx) || ((object_idx == other.object_idx) && (volume_idx < other.volume_idx)); }
        bool operator == (const SelectedVolume& other) const { return (object_idx == other.object_idx) && (volume_idx == other.volume_idx); }
    };
    std::vector<SelectedVolume> selected_volumes;

    const Selection& selection = get_selection();

    // collects selected ModelVolumes
    const std::set<unsigned int>& selected_volumes_idxs = selection.get_volume_idxs();
    for (unsigned int idx : selected_volumes_idxs) {
        const GLVolume* v = selection.get_volume(idx);
        int v_idx = v->volume_idx();
        if (v_idx >= 0) {
            int o_idx = v->object_idx();
            if (0 <= o_idx && o_idx < (int)model.objects.size())
                selected_volumes.push_back({ o_idx, v_idx });
        }
    }
#endif // ENABLE_RELOAD_FROM_DISK_REWORK

#if ENABLE_RELOAD_FROM_DISK_REWORK
    std::sort(selected_volumes.begin(), selected_volumes.end(), [](const std::pair<int, int> &v1, const std::pair<int, int> &v2) {
        return (v1.first < v2.first) || (v1.first == v2.first && v1.second < v2.second);
        });
    selected_volumes.erase(std::unique(selected_volumes.begin(), selected_volumes.end(), [](const std::pair<int, int> &v1, const std::pair<int, int> &v2) {
        return (v1.first == v2.first) && (v1.second == v2.second);
        }), selected_volumes.end());

    // collects paths of files to load
    std::vector<fs::path> paths;
    for (auto [obj_idx, vol_idx] : selected_volumes) {
        paths.push_back(model.objects[obj_idx]->volumes[vol_idx]->source.input_file);
    }
#else
    std::sort(selected_volumes.begin(), selected_volumes.end());
    selected_volumes.erase(std::unique(selected_volumes.begin(), selected_volumes.end()), selected_volumes.end());

    // collects paths of files to load
    std::vector<fs::path> paths;
    for (const SelectedVolume& v : selected_volumes) {
        const ModelObject* object = model.objects[v.object_idx];
        const ModelVolume* volume = object->volumes[v.volume_idx];
        if (!volume->source.input_file.empty())
            paths.push_back(volume->source.input_file);
        else if (!object->input_file.empty() && !volume->name.empty() && !volume->source.is_from_builtin_objects)
            paths.push_back(volume->name);
    }
#endif // ENABLE_RELOAD_FROM_DISK_REWORK
    std::sort(paths.begin(), paths.end());
    paths.erase(std::unique(paths.begin(), paths.end()), paths.end());

    return !paths.empty();
}

void Plater::priv::update_publish_dialog_status(wxString &msg, int percent)
{
    if (m_publish_dlg)
        m_publish_dlg->UpdateStatus(msg, percent);
}

bool Plater::priv::show_publish_dlg(bool show)
{
    if (q != nullptr) { BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << ":recevied publish event\n"; }

    if (!m_publish_dlg) m_publish_dlg = new PublishDialog(q);
    if (show) {
        m_publish_dlg->reset();
        m_publish_dlg->start_slicing();
        //m_publish_dlg->Show();
        m_publish_dlg->ShowModal();
    } else {
        m_publish_dlg->EndModal(wxID_OK);
        //cancel the slicing
        if (this->background_process.running())
            this->background_process.stop();
    }
    return true;
}

//BBS: add bed exclude area
void Plater::priv::set_bed_shape(const Pointfs& shape, const Pointfs& exclude_areas, const double printable_height, const std::string& custom_texture, const std::string& custom_model, bool force_as_custom)
{
    //Orca: reduce resolution for large bed printer
    BoundingBoxf bed_size = get_extents(shape);
    if (bed_size.size().maxCoeff() <= LARGE_BED_THRESHOLD)
        SCALING_FACTOR = SCALING_FACTOR_INTERNAL;
    else
        SCALING_FACTOR = SCALING_FACTOR_INTERNAL_LARGE_PRINTER;

    //BBS: add shape position
    Vec2d shape_position = partplate_list.get_current_shape_position();
    bool new_shape = bed.set_shape(shape, printable_height, custom_model, force_as_custom, shape_position);

    float prev_height_lid, prev_height_rod;
    partplate_list.get_height_limits(prev_height_lid, prev_height_rod);
    double height_to_lid = config->opt_float("extruder_clearance_height_to_lid");
    double height_to_rod = config->opt_float("extruder_clearance_height_to_rod");

    Pointfs prev_exclude_areas = partplate_list.get_exclude_area();
    new_shape |= (height_to_lid != prev_height_lid) || (height_to_rod != prev_height_rod) || (prev_exclude_areas != exclude_areas);
    if (!new_shape && partplate_list.get_logo_texture_filename() != custom_texture) {
        partplate_list.update_logo_texture_filename(custom_texture);
    }
    if (new_shape) {
        if (view3D) view3D->bed_shape_changed();
        if (preview) preview->bed_shape_changed();

        //BBS: update part plate's size
        // BBS: to be checked
        Vec3d max = bed.extended_bounding_box().max;
        Vec3d min = bed.extended_bounding_box().min;
        double z = config->opt_float("printable_height");

        //Pointfs& exclude_areas = config->option<ConfigOptionPoints>("bed_exclude_area")->values;
        partplate_list.reset_size(max.x() - min.x() - Bed3D::Axes::DefaultTipRadius, max.y() - min.y() - Bed3D::Axes::DefaultTipRadius, z);
        partplate_list.set_shapes(shape, exclude_areas, custom_texture, height_to_lid, height_to_rod);

        Vec2d new_shape_position = partplate_list.get_current_shape_position();
        if (shape_position != new_shape_position)
            bed.set_shape(shape, printable_height, custom_model, force_as_custom, new_shape_position);
    }
}

bool Plater::priv::can_delete() const
{
    return !get_selection().is_empty() && !get_selection().is_wipe_tower();
}

bool Plater::priv::can_delete_all() const
{
    return !model.objects.empty();
}

bool Plater::priv::can_add_plate() const
{
    return q->get_partplate_list().get_plate_count() < PartPlateList::MAX_PLATES_COUNT;
}

bool Plater::priv::can_delete_plate() const
{
    return q->get_partplate_list().get_plate_count() > 1;
}

bool Plater::priv::can_fix_through_netfabb() const
{
    std::vector<int> obj_idxs, vol_idxs;
    sidebar->obj_list()->get_selection_indexes(obj_idxs, vol_idxs);

#if FIX_THROUGH_NETFABB_ALWAYS
    // Fixing always.
    return ! obj_idxs.empty() || ! vol_idxs.empty();
#else // FIX_THROUGH_NETFABB_ALWAYS
    // Fixing only if the model is not manifold.
    if (vol_idxs.empty()) {
        for (auto obj_idx : obj_idxs)
            if (model.objects[obj_idx]->get_repaired_errors_count() > 0)
                return true;
        return false;
    }

    int obj_idx = obj_idxs.front();
    for (auto vol_idx : vol_idxs)
        if (model.objects[obj_idx]->get_repaired_errors_count(vol_idx) > 0)
            return true;
    return false;
#endif // FIX_THROUGH_NETFABB_ALWAYS
}

bool Plater::priv::can_simplify() const
{
    // is object for simplification selected
    if (get_selected_object_idx() < 0) return false;
    // is already opened?
    if (q->get_view3D_canvas3D()->get_gizmos_manager().get_current_type() ==
        GLGizmosManager::EType::Simplify)
        return false;
    return true;
}

bool Plater::priv::can_increase_instances() const
{
    if (!m_worker.is_idle()
     || q->get_view3D_canvas3D()->get_gizmos_manager().is_in_editing_mode())
            return false;

    int obj_idx = get_selected_object_idx();
    return (0 <= obj_idx) && (obj_idx < (int)model.objects.size())
        && !sidebar->obj_list()->has_selected_cut_object()
        && std::all_of(model.objects[obj_idx]->instances.begin(), model.objects[obj_idx]->instances.end(), [](auto& inst) {return inst->printable; });
}

bool Plater::priv::can_decrease_instances() const
{
    if (!m_worker.is_idle()
     || q->get_view3D_canvas3D()->get_gizmos_manager().is_in_editing_mode())
            return false;

    int obj_idx = get_selected_object_idx();
    return (0 <= obj_idx) && (obj_idx < (int)model.objects.size()) && (model.objects[obj_idx]->instances.size() > 1)
        && !sidebar->obj_list()->has_selected_cut_object();
}

bool Plater::priv::can_split_to_objects() const
{
    return q->can_split(true);
}

bool Plater::priv::can_split_to_volumes() const
{
    return (printer_technology != ptSLA) && q->can_split(false);
}

bool Plater::priv::can_arrange() const
{
    return !model.objects.empty() && m_worker.is_idle();
}

bool Plater::priv::layers_height_allowed() const
{
    if (printer_technology != ptFFF)
        return false;

    int obj_idx = get_selected_object_idx();
    return 0 <= obj_idx && obj_idx < (int)model.objects.size() && model.objects[obj_idx]->max_z() > SINKING_Z_THRESHOLD && view3D->is_layers_editing_allowed();
}

bool Plater::priv::can_layers_editing() const
{
    return layers_height_allowed();
}

void Plater::priv::on_action_layersediting(SimpleEvent&)
{
    const bool enabling = !view3D->is_layers_editing_enabled();
    view3D->enable_layers_editing(enabling);
    notification_manager->set_move_from_overlay(view3D->is_layers_editing_enabled());
    if (enabling) {
        bool has_local_z = false;
        if (const auto* opt = wxGetApp().preset_bundle->project_config.option<ConfigOptionBool>("dithering_local_z_mode"))
            has_local_z = opt->value;
        q->notify_vhl_dithering_conflict(has_local_z);
    }
}

/**
 * Show a warning when both Variable Layer Height and Subdivide Mix Layer
 * (dithering_local_z_mode) are enabled, as both features alter layer
 * heights and may produce unexpected results.
 * @param local_z_enabled true if the Subdivide Mix Layer setting is active
 */
bool Plater::has_incompatible_mixed_filament_in_use() const
{
    return p && p->has_incompatible_mixed_filament_in_use();
}

void Plater::notify_vhl_dithering_conflict(bool local_z_enabled)
{
    if (!local_z_enabled)
        return;

    if (!p) return;

    bool has_adaptive = p->view3D->is_layers_editing_enabled();
    if (!has_adaptive) {
        for (const auto* obj : p->model.objects) {
            if (!obj->layer_height_profile.empty()) {
                has_adaptive = true;
                break;
            }
        }
    }

    if (!has_adaptive)
        return;

    MessageDialog dialog(this,
        _L("Cannot enable both Variable Layer Height and Subdivide Mix Layer."),
        _L("Warning"), wxICON_WARNING | wxOK);
    dialog.ShowModal();
}

void Plater::priv::on_create_filament(SimpleEvent &)
{
    CreateFilamentPresetDialog dlg(wxGetApp().mainframe);
    int res = dlg.ShowModal();
    if (wxID_OK == res) {
        wxGetApp().mainframe->update_side_preset_ui();
        update_ui_from_settings();
        sidebar->update_all_preset_comboboxes();
        CreatePresetSuccessfulDialog success_dlg(wxGetApp().mainframe, SuccessType::FILAMENT);
        int                          res = success_dlg.ShowModal();
    }
}

void Plater::priv::on_modify_filament(SimpleEvent &evt)
{
    Filamentinformation *filament_info = static_cast<Filamentinformation *>(evt.GetEventObject());
    int                 res;
    std::shared_ptr<Preset> need_edit_preset;
    {
        EditFilamentPresetDialog dlg(wxGetApp().mainframe, filament_info);
        res = dlg.ShowModal();
        need_edit_preset = dlg.get_need_edit_preset();
    }
    wxGetApp().mainframe->update_side_preset_ui();
    update_ui_from_settings();
    sidebar->update_all_preset_comboboxes();
    if (wxID_EDIT == res) {
        Tab *tab = wxGetApp().get_tab(Preset::Type::TYPE_FILAMENT);
        //tab->restore_last_select_item();
        if (tab == nullptr) { return; }
        // Popup needs to be called before "restore_last_select_item", otherwise the page may not be updated
        wxGetApp().params_dialog()->Popup();
        tab->restore_last_select_item();
        // Opening Studio and directly accessing the Filament settings interface through the edit preset button will not take effect and requires manual settings.
        tab->set_just_edit(true);
        tab->select_preset(need_edit_preset->name);
        // when some preset have modified, if the printer is not need_edit_preset_name compatible printer, the preset will jump to other preset, need select again
        if (!need_edit_preset->is_compatible) tab->select_preset(need_edit_preset->name);
    }

}

void Plater::priv::on_add_filament(SimpleEvent &evt) {
    sidebar->add_filament();
}

void Plater::priv::on_delete_filament(SimpleEvent &evt) {
    sidebar->delete_filament();
}

void Plater::priv::on_add_custom_filament(ColorEvent &evt)
{
    sidebar->add_custom_filament(evt.data);
}

void Plater::priv::enter_gizmos_stack()
{
    assert(m_undo_redo_stack_active == &m_undo_redo_stack_main);
    if (m_undo_redo_stack_active == &m_undo_redo_stack_main) {
        m_undo_redo_stack_active = &m_undo_redo_stack_gizmos;
        assert(m_undo_redo_stack_active->empty());
        // Take the initial snapshot of the gizmos.
        // Not localized on purpose, the text will never be shown to the user.
        this->take_snapshot(std::string("Gizmos-Initial"));
    }
}

bool Plater::priv::leave_gizmos_stack()
{
    bool changed = false;
    assert(m_undo_redo_stack_active == &m_undo_redo_stack_gizmos);
    if (m_undo_redo_stack_active == &m_undo_redo_stack_gizmos) {
        assert(! m_undo_redo_stack_active->empty());
        changed = m_undo_redo_stack_gizmos.has_undo_snapshot();
        m_undo_redo_stack_active->clear();
        m_undo_redo_stack_active = &m_undo_redo_stack_main;
    }
    return changed;
}

int Plater::priv::get_active_snapshot_index()
{
    const size_t active_snapshot_time = this->undo_redo_stack().active_snapshot_time();
    const std::vector<UndoRedo::Snapshot>& ss_stack = this->undo_redo_stack().snapshots();
    const auto it = std::lower_bound(ss_stack.begin(), ss_stack.end(), UndoRedo::Snapshot(active_snapshot_time));
    return it - ss_stack.begin();
}

void Plater::priv::take_snapshot(const std::string& snapshot_name, const UndoRedo::SnapshotType snapshot_type)
{
    if (m_prevent_snapshots > 0)
        return;
    assert(m_prevent_snapshots >= 0);
    // BBS: single snapshot
    if (m_single && !m_single->check(snapshot_modifies_project(snapshot_type) && (snapshot_name.empty() || snapshot_name.back() != '!')))
        return;
    UndoRedo::SnapshotData snapshot_data;
    snapshot_data.snapshot_type      = snapshot_type;
    snapshot_data.printer_technology = this->printer_technology;
    if (this->view3D->is_layers_editing_enabled())
        snapshot_data.flags |= UndoRedo::SnapshotData::VARIABLE_LAYER_EDITING_ACTIVE;
    if (this->sidebar->obj_list()->is_selected(itSettings)) {
        snapshot_data.flags |= UndoRedo::SnapshotData::SELECTED_SETTINGS_ON_SIDEBAR;
        snapshot_data.layer_range_idx = this->sidebar->obj_list()->get_selected_layers_range_idx();
    }
    else if (this->sidebar->obj_list()->is_selected(itLayer)) {
        snapshot_data.flags |= UndoRedo::SnapshotData::SELECTED_LAYER_ON_SIDEBAR;
        snapshot_data.layer_range_idx = this->sidebar->obj_list()->get_selected_layers_range_idx();
    }
    else if (this->sidebar->obj_list()->is_selected(itLayerRoot))
        snapshot_data.flags |= UndoRedo::SnapshotData::SELECTED_LAYERROOT_ON_SIDEBAR;

    // If SLA gizmo is active, ask it if it wants to trigger support generation
    // on loading this snapshot.
    if (view3D->get_canvas3d()->get_gizmos_manager().wants_reslice_supports_on_undo())
        snapshot_data.flags |= UndoRedo::SnapshotData::RECALCULATE_SLA_SUPPORTS;

    //FIXME updating the Wipe tower config values at the ModelWipeTower from the Print config.
    // This is a workaround until we refactor the Wipe Tower position / orientation to live solely inside the Model, not in the Print config.
    // BBS: add partplate logic
    if (this->printer_technology == ptFFF) {
        const DynamicPrintConfig& config = wxGetApp().preset_bundle->prints.get_edited_preset().config;
        const DynamicPrintConfig& proj_cfg = wxGetApp().preset_bundle->project_config;
        const ConfigOptionFloats* tower_x_opt = proj_cfg.option<ConfigOptionFloats>("wipe_tower_x");
        const ConfigOptionFloats* tower_y_opt = proj_cfg.option<ConfigOptionFloats>("wipe_tower_y");
        assert(tower_x_opt->values.size() == tower_y_opt->values.size());
        model.wipe_tower.positions.clear();
        model.wipe_tower.positions.resize(tower_x_opt->values.size());
        for (int plate_idx = 0; plate_idx < tower_x_opt->values.size(); plate_idx++) {
            ModelWipeTower& tower = model.wipe_tower;

            tower.positions[plate_idx] = Vec2d(tower_x_opt->get_at(plate_idx), tower_y_opt->get_at(plate_idx));
            tower.rotation = proj_cfg.opt_float("wipe_tower_rotation_angle");
        }
    }
    GLCanvas3D* canvas = get_current_canvas3D();
    if (!canvas) {
        // During destruction, skip snapshot
        return;
    }
    
    const GLGizmosManager& gizmos = canvas->get_canvas_type() == GLCanvas3D::CanvasAssembleView ? assemble_view->get_canvas3d()->get_gizmos_manager() : view3D->get_canvas3d()->get_gizmos_manager();

    if (snapshot_type == UndoRedo::SnapshotType::ProjectSeparator)
        this->undo_redo_stack().clear();
    this->undo_redo_stack().take_snapshot(snapshot_name, model, get_current_canvas3D()->get_canvas_type() == GLCanvas3D::CanvasAssembleView ? assemble_view->get_canvas3d()->get_selection() : view3D->get_canvas3d()->get_selection(), gizmos, partplate_list, snapshot_data);
    if (snapshot_type == UndoRedo::SnapshotType::LeavingGizmoWithAction) {
        // Filter all but the last UndoRedo::SnapshotType::GizmoAction in a row between the last UndoRedo::SnapshotType::EnteringGizmo and UndoRedo::SnapshotType::LeavingGizmoWithAction.
        // The remaining snapshot will be renamed to a more generic name,
        // depending on what gizmo is being left.
        if (gizmos.get_current() != nullptr) {
            std::string new_name = gizmos.get_current()->get_action_snapshot_name();
            this->undo_redo_stack().reduce_noisy_snapshots(new_name);
        }
    } else if (snapshot_type == UndoRedo::SnapshotType::ProjectSeparator) {
        // Reset the "dirty project" flag.
        m_undo_redo_stack_main.mark_current_as_saved();
    }
    //BBS: add PartPlateList as the paremeter for take_snapshot
    this->undo_redo_stack().release_least_recently_used();

    dirty_state.update_from_undo_redo_stack(m_undo_redo_stack_main.project_modified());

    // Save the last active preset name of a particular printer technology.
    ((this->printer_technology == ptFFF) ? m_last_fff_printer_profile_name : m_last_sla_printer_profile_name) = wxGetApp().preset_bundle->printers.get_selected_preset_name();
    BOOST_LOG_TRIVIAL(info) << "Undo / Redo snapshot taken: " << snapshot_name << ", Undo / Redo stack memory: " << Slic3r::format_memsize_MB(this->undo_redo_stack().memsize()) << log_memory_info();
}

void Plater::priv::undo()
{
    const std::vector<UndoRedo::Snapshot> &snapshots = this->undo_redo_stack().snapshots();
    auto it_current = std::lower_bound(snapshots.begin(), snapshots.end(), UndoRedo::Snapshot(this->undo_redo_stack().active_snapshot_time()));
    // BBS: undo-redo until modify record
    while (--it_current != snapshots.begin() && !snapshot_modifies_project(*it_current));
    if (it_current == snapshots.begin()) return;
    GLCanvas3D* canvas = get_current_canvas3D();
    if (!canvas) return;
    if (canvas->get_canvas_type() == GLCanvas3D::CanvasAssembleView) {
        if (it_current->snapshot_data.snapshot_type != UndoRedo::SnapshotType::GizmoAction &&
            it_current->snapshot_data.snapshot_type != UndoRedo::SnapshotType::EnteringGizmo &&
            it_current->snapshot_data.snapshot_type != UndoRedo::SnapshotType::LeavingGizmoNoAction &&
            it_current->snapshot_data.snapshot_type != UndoRedo::SnapshotType::LeavingGizmoWithAction)
            return;
    }
    this->undo_redo_to(it_current);
}

void Plater::priv::redo()
{
    const std::vector<UndoRedo::Snapshot> &snapshots = this->undo_redo_stack().snapshots();
    auto it_current = std::lower_bound(snapshots.begin(), snapshots.end(), UndoRedo::Snapshot(this->undo_redo_stack().active_snapshot_time()));
    // BBS: undo-redo until modify record
    while (it_current != snapshots.end() && !snapshot_modifies_project(*it_current++));
    if (it_current != snapshots.end()) {
        while (it_current != snapshots.end() && !snapshot_modifies_project(*it_current++));
        this->undo_redo_to(--it_current);
    }
}

void Plater::priv::undo_redo_to(size_t time_to_load)
{
    const std::vector<UndoRedo::Snapshot> &snapshots = this->undo_redo_stack().snapshots();
    auto it_current = std::lower_bound(snapshots.begin(), snapshots.end(), UndoRedo::Snapshot(time_to_load));
    assert(it_current != snapshots.end());
    this->undo_redo_to(it_current);
}

// BBS: check need save or backup
bool Plater::priv::up_to_date(bool saved, bool backup)
{
    size_t& last_time = backup ? m_backup_timestamp : m_saved_timestamp;
    if (saved) {
        last_time = undo_redo_stack_main().active_snapshot_time();
        if (!backup)
            undo_redo_stack_main().mark_current_as_saved();
        return true;
    }
    else {
        return !undo_redo_stack_main().has_real_change_from(last_time);
    }
}

void Plater::priv::undo_redo_to(std::vector<UndoRedo::Snapshot>::const_iterator it_snapshot)
{
    // Make sure that no updating function calls take_snapshot until we are done.
    SuppressSnapshots snapshot_supressor(q);

    bool 				temp_snapshot_was_taken 	= this->undo_redo_stack().temp_snapshot_active();
    PrinterTechnology 	new_printer_technology 		= it_snapshot->snapshot_data.printer_technology;
    bool 				printer_technology_changed 	= this->printer_technology != new_printer_technology;
    if (printer_technology_changed) {
        //BBS do not support SLA
    }
    // Save the last active preset name of a particular printer technology.
    ((this->printer_technology == ptFFF) ? m_last_fff_printer_profile_name : m_last_sla_printer_profile_name) = wxGetApp().preset_bundle->printers.get_selected_preset_name();
    //FIXME updating the Wipe tower config values at the ModelWipeTower from the Print config.
    // This is a workaround until we refactor the Wipe Tower position / orientation to live solely inside the Model, not in the Print config.
    // BBS: add partplate logic
    if (this->printer_technology == ptFFF) {
        const DynamicPrintConfig& config = wxGetApp().preset_bundle->prints.get_edited_preset().config;
        const DynamicPrintConfig& proj_cfg = wxGetApp().preset_bundle->project_config;
        const ConfigOptionFloats* tower_x_opt = proj_cfg.option<ConfigOptionFloats>("wipe_tower_x");
        const ConfigOptionFloats* tower_y_opt = proj_cfg.option<ConfigOptionFloats>("wipe_tower_y");
        assert(tower_x_opt->values.size() == tower_y_opt->values.size());
        model.wipe_tower.positions.clear();
        model.wipe_tower.positions.resize(tower_x_opt->values.size());
        for (int plate_idx = 0; plate_idx < tower_x_opt->values.size(); plate_idx++) {
            ModelWipeTower& tower = model.wipe_tower;

            tower.positions[plate_idx] = Vec2d(tower_x_opt->get_at(plate_idx), tower_y_opt->get_at(plate_idx));
            tower.rotation = proj_cfg.opt_float("wipe_tower_rotation_angle");
        }
    }
    const int layer_range_idx = it_snapshot->snapshot_data.layer_range_idx;
    // Flags made of Snapshot::Flags enum values.
    unsigned int new_flags = it_snapshot->snapshot_data.flags;
    UndoRedo::SnapshotData top_snapshot_data;
    top_snapshot_data.printer_technology = this->printer_technology;
    if (this->view3D->is_layers_editing_enabled())
        top_snapshot_data.flags |= UndoRedo::SnapshotData::VARIABLE_LAYER_EDITING_ACTIVE;
    if (this->sidebar->obj_list()->is_selected(itSettings)) {
        top_snapshot_data.flags |= UndoRedo::SnapshotData::SELECTED_SETTINGS_ON_SIDEBAR;
        top_snapshot_data.layer_range_idx = this->sidebar->obj_list()->get_selected_layers_range_idx();
    }
    else if (this->sidebar->obj_list()->is_selected(itLayer)) {
        top_snapshot_data.flags |= UndoRedo::SnapshotData::SELECTED_LAYER_ON_SIDEBAR;
        top_snapshot_data.layer_range_idx = this->sidebar->obj_list()->get_selected_layers_range_idx();
    }
    else if (this->sidebar->obj_list()->is_selected(itLayerRoot))
        top_snapshot_data.flags |= UndoRedo::SnapshotData::SELECTED_LAYERROOT_ON_SIDEBAR;
    bool   		 new_variable_layer_editing_active = (new_flags & UndoRedo::SnapshotData::VARIABLE_LAYER_EDITING_ACTIVE) != 0;
    bool         new_selected_settings_on_sidebar  = (new_flags & UndoRedo::SnapshotData::SELECTED_SETTINGS_ON_SIDEBAR) != 0;
    bool         new_selected_layer_on_sidebar     = (new_flags & UndoRedo::SnapshotData::SELECTED_LAYER_ON_SIDEBAR) != 0;
    bool         new_selected_layerroot_on_sidebar = (new_flags & UndoRedo::SnapshotData::SELECTED_LAYERROOT_ON_SIDEBAR) != 0;

    if (this->view3D->get_canvas3d()->get_gizmos_manager().wants_reslice_supports_on_undo())
        top_snapshot_data.flags |= UndoRedo::SnapshotData::RECALCULATE_SLA_SUPPORTS;

    // Disable layer editing before the Undo / Redo jump.
    if (!new_variable_layer_editing_active && view3D->is_layers_editing_enabled())
        view3D->get_canvas3d()->force_main_toolbar_left_action(view3D->get_canvas3d()->get_main_toolbar_item_id("layersediting"));

    // Make a copy of the snapshot, undo/redo could invalidate the iterator
    const UndoRedo::Snapshot snapshot_copy = *it_snapshot;
    // Do the jump in time.
    GLCanvas3D* canvas = get_current_canvas3D();
    if (!canvas) return;
    
    bool is_assemble = canvas->get_canvas_type() == GLCanvas3D::CanvasAssembleView;
    if (it_snapshot->timestamp < this->undo_redo_stack().active_snapshot_time() ?
        this->undo_redo_stack().undo(model, is_assemble ? assemble_view->get_canvas3d()->get_selection() : this->view3D->get_canvas3d()->get_selection(), is_assemble ? assemble_view->get_canvas3d()->get_gizmos_manager() : this->view3D->get_canvas3d()->get_gizmos_manager(), this->partplate_list, top_snapshot_data, it_snapshot->timestamp) :
        this->undo_redo_stack().redo(model, is_assemble ? assemble_view->get_canvas3d()->get_gizmos_manager() : this->view3D->get_canvas3d()->get_gizmos_manager(), this->partplate_list, it_snapshot->timestamp)) {
        if (printer_technology_changed) {
            // Switch to the other printer technology. Switch to the last printer active for that particular technology.
            AppConfig *app_config = wxGetApp().app_config;
            app_config->set("presets", PRESET_PRINTER_NAME, (new_printer_technology == ptFFF) ? m_last_fff_printer_profile_name : m_last_sla_printer_profile_name);
            //FIXME Why are we reloading the whole preset bundle here? Please document. This is fishy and it is unnecessarily expensive.
            // Anyways, don't report any config value substitutions, they have been already reported to the user at application start up.
            wxGetApp().preset_bundle->load_presets(*app_config, ForwardCompatibilitySubstitutionRule::EnableSilent);
            // load_current_presets() calls Tab::load_current_preset() -> TabPrint::update() -> Object_list::update_and_show_object_settings_item(),
            // but the Object list still keeps pointer to the old Model. Avoid a crash by removing selection first.
            this->sidebar->obj_list()->unselect_objects();
            // Load the currently selected preset into the GUI, update the preset selection box.
            // This also switches the printer technology based on the printer technology of the active printer profile.
            wxGetApp().load_current_presets();
        }
        //FIXME updating the Print config from the Wipe tower config values at the ModelWipeTower.
        // This is a workaround until we refactor the Wipe Tower position / orientation to live solely inside the Model, not in the Print config.
        // BBS: add partplate logic
        if (this->printer_technology == ptFFF) {
            const DynamicPrintConfig& config = wxGetApp().preset_bundle->prints.get_edited_preset().config;
            const DynamicPrintConfig& proj_cfg = wxGetApp().preset_bundle->project_config;
            ConfigOptionFloats* tower_x_opt = const_cast<ConfigOptionFloats*>(proj_cfg.option<ConfigOptionFloats>("wipe_tower_x"));
            ConfigOptionFloats* tower_y_opt = const_cast<ConfigOptionFloats*>(proj_cfg.option<ConfigOptionFloats>("wipe_tower_y"));
            // BBS: don't support wipe tower rotation
            //double current_rotation = proj_cfg.opt_float("wipe_tower_rotation_angle");
            bool need_update = false;
            if (tower_x_opt->values.size() != model.wipe_tower.positions.size()) {
                tower_x_opt->clear();
                ConfigOptionFloat default_tower_x(40.f);
                tower_x_opt->resize(model.wipe_tower.positions.size(), &default_tower_x);
                need_update = true;
            }

            if (tower_y_opt->values.size() != model.wipe_tower.positions.size()) {
                tower_y_opt->clear();
                ConfigOptionFloat default_tower_y(200.f);
                tower_y_opt->resize(model.wipe_tower.positions.size(), &default_tower_y);
                need_update = true;
            }

            for (int plate_idx = 0; plate_idx < model.wipe_tower.positions.size(); plate_idx++) {
                if (Vec2d(tower_x_opt->get_at(plate_idx), tower_y_opt->get_at(plate_idx)) != model.wipe_tower.positions[plate_idx]) {
                    ConfigOptionFloat tower_x_new(model.wipe_tower.positions[plate_idx].x());
                    ConfigOptionFloat tower_y_new(model.wipe_tower.positions[plate_idx].y());
                    tower_x_opt->set_at(&tower_x_new, plate_idx, 0);
                    tower_y_opt->set_at(&tower_y_new, plate_idx, 0);
                    need_update = true;
                    break;
                }
            }

            if (need_update) {
                // update print to current plate (preview->m_process)
                this->partplate_list.update_slice_context_to_current_plate(this->background_process);
                this->preview->update_gcode_result(this->partplate_list.get_current_slice_result());
                this->update();
            }
        }
        // set selection mode for ObjectList on sidebar
        this->sidebar->obj_list()->set_selection_mode(new_selected_settings_on_sidebar  ? ObjectList::SELECTION_MODE::smSettings :
                                                      new_selected_layer_on_sidebar     ? ObjectList::SELECTION_MODE::smLayer :
                                                      new_selected_layerroot_on_sidebar ? ObjectList::SELECTION_MODE::smLayerRoot :
                                                                                          ObjectList::SELECTION_MODE::smUndef);
        if (new_selected_settings_on_sidebar || new_selected_layer_on_sidebar)
            this->sidebar->obj_list()->set_selected_layers_range_idx(layer_range_idx);

        this->update_after_undo_redo(snapshot_copy, temp_snapshot_was_taken);
        // Enable layer editing after the Undo / Redo jump.
        if (!view3D->is_layers_editing_enabled() && this->layers_height_allowed() && new_variable_layer_editing_active)
            view3D->get_canvas3d()->force_main_toolbar_left_action(view3D->get_canvas3d()->get_main_toolbar_item_id("layersediting"));
    }

    dirty_state.update_from_undo_redo_stack(m_undo_redo_stack_main.project_modified());
    update_title_dirty_status();
}

void Plater::priv::update_after_undo_redo(const UndoRedo::Snapshot& snapshot, bool /* temp_snapshot_was_taken */)
{
    GLCanvas3D* canvas = get_current_canvas3D();
    if (!canvas) return;
    
    bool is_assemble = canvas->get_canvas_type() == GLCanvas3D::CanvasAssembleView;
    is_assemble ? assemble_view->get_canvas3d()->get_selection().clear() : this->view3D->get_canvas3d()->get_selection().clear();
    // Update volumes from the deserializd model, always stop / update the background processing (for both the SLA and FFF technologies).
    this->update((unsigned int)UpdateParams::FORCE_BACKGROUND_PROCESSING_UPDATE | (unsigned int)UpdateParams::POSTPONE_VALIDATION_ERROR_MESSAGE);
    // Release old snapshots if the memory allocated is excessive. This may remove the top most snapshot if jumping to the very first snapshot.
    //if (temp_snapshot_was_taken)
    // Release the old snapshots always, as it may have happened, that some of the triangle meshes got deserialized from the snapshot, while some
    // triangle meshes may have gotten released from the scene or the background processing, therefore now being calculated into the Undo / Redo stack size.
        this->undo_redo_stack().release_least_recently_used();
    //YS_FIXME update obj_list from the deserialized model (maybe store ObjectIDs into the tree?) (no selections at this point of time)
        is_assemble ?
            assemble_view->get_canvas3d()->get_selection().set_deserialized(GUI::Selection::EMode(this->undo_redo_stack().selection_deserialized().mode), this->undo_redo_stack().selection_deserialized().volumes_and_instances) :
            this->view3D->get_canvas3d()->get_selection().set_deserialized(GUI::Selection::EMode(this->undo_redo_stack().selection_deserialized().mode), this->undo_redo_stack().selection_deserialized().volumes_and_instances);
    is_assemble ?
        assemble_view->get_canvas3d()->get_gizmos_manager().update_after_undo_redo(snapshot) :
        this->view3D->get_canvas3d()->get_gizmos_manager().update_after_undo_redo(snapshot);

    wxGetApp().obj_list()->update_after_undo_redo();

    if (wxGetApp().get_mode() == comSimple && model_has_advanced_features(this->model)) {
        // If the user jumped to a snapshot that require user interface with advanced features, switch to the advanced mode without asking.
        // There is a little risk of surprising the user, as he already must have had the advanced or advanced mode active for such a snapshot to be taken.
        Slic3r::GUI::wxGetApp().save_mode(comAdvanced);
        view3D->set_as_dirty();
    }

    // this->update() above was called with POSTPONE_VALIDATION_ERROR_MESSAGE, so that if an error message was generated when updating the back end, it would not open immediately,
    // but it would be saved to be show later. Let's do it now. We do not want to display the message box earlier, because on Windows & OSX the message box takes over the message
    // queue pump, which in turn executes the rendering function before a full update after the Undo / Redo jump.
    this->show_delayed_error_message();

    //FIXME what about the state of the manipulators?
    //FIXME what about the focus? Cursor in the side panel?

    BOOST_LOG_TRIVIAL(info) << "Undo / Redo snapshot reloaded. Undo / Redo stack memory: " << Slic3r::format_memsize_MB(this->undo_redo_stack().memsize()) << log_memory_info();
}

void Plater::priv::bring_instance_forward() const
{
#ifdef __APPLE__
    wxGetApp().other_instance_message_handler()->bring_instance_forward();
    return;
#endif //__APPLE__
    if (main_frame == nullptr) {
        BOOST_LOG_TRIVIAL(debug) << "Couldnt bring instance forward - mainframe is null";
        return;
    }
    BOOST_LOG_TRIVIAL(debug) << "Snapmaker Orca window going forward";
    //this code maximize app window on Fedora
    {
        main_frame->Iconize(false);
        if (main_frame->IsMaximized())
            main_frame->Maximize(true);
        else
            main_frame->Maximize(false);
    }
    //this code maximize window on Ubuntu
    {
        main_frame->Restore();
        wxGetApp().GetTopWindow()->SetFocus();  // focus on my window
        wxGetApp().GetTopWindow()->Raise();  // bring window to front
        wxGetApp().GetTopWindow()->Show(true); // show the window
    }
}

//BBS: popup object table
bool Plater::priv::PopupObjectTable(int object_id, int volume_id, const wxPoint& position)
{
    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(" enter, create ObjectTableDialog");
    int max_width{1920}, max_height{1080};

    max_width = q->GetMaxWidth();
    max_height = q->GetMaxHeight();
    ObjectTableDialog table_dialog(q, q, &model, wxSize(max_width, max_height));
    //m_popup_table = new ObjectTableDialog(q, q,  &model);

    wxRect rect = sidebar->GetRect();
    wxPoint pos = sidebar->ClientToScreen(wxPoint(rect.x, rect.y));

    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": show ObjectTableDialog");
    table_dialog.Popup(object_id, volume_id, pos);

    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(" finished, will destroy ObjectTableDialog");
    return true;
}


void Plater::priv::record_start_print_preset(std::string action) {
    // record start print preset
    try {
        json j;
        j["user_mode"] = wxGetApp().get_mode_str();
        int  plate_count = partplate_list.get_plate_count();
        j["plate_count"] = plate_count;
        unsigned int obj_count = model.objects.size();
        j["obj_count"] = obj_count;
        auto printer_preset = wxGetApp().preset_bundle->printers.get_edited_preset_with_vendor_profile().preset;
        if (printer_preset.is_system) {
            j["printer_preset_name"] = printer_preset.name;
        }
        else {
            j["printer_preset_name"] = printer_preset.config.opt_string("inherits");
        }
        auto filament_presets = wxGetApp().preset_bundle->filament_presets;
        for (int i = 0; i < filament_presets.size(); ++i) {
            auto filament_preset = wxGetApp().preset_bundle->filaments.find_preset(filament_presets[i]);
            if (filament_preset->is_system) {
                j["filament_preset_" + std::to_string(i)] = filament_preset->name;
            }
            else {
                j["filament_preset_" + std::to_string(i)] = filament_preset->config.opt_string("inherits");
            }
        }

        Preset& print_preset = wxGetApp().preset_bundle->prints.get_edited_preset();
        if (print_preset.is_system) {
            j["process_preset"] = print_preset.name;
        }
        else {
            j["process_preset"] = print_preset.config.opt_string("inherits");
        }

        j["record_event"] = action;
        NetworkAgent* agent = wxGetApp().getAgent();
    }
    catch (...) {
        return;
    }

}

void Sidebar::set_btn_label(const ActionButtonType btn_type, const wxString& label) const
{
    switch (btn_type)
    {
        case ActionButtonType::abReslice:   p->btn_reslice->SetLabelText(label);        break;
        case ActionButtonType::abExport:    p->btn_export_gcode->SetLabelText(label);   break;
        case ActionButtonType::abSendGCode: /*p->btn_send_gcode->SetLabelText(label);*/     break;
    }
}

// Plater / Public

Plater::Plater(wxWindow *parent, MainFrame *main_frame)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxGetApp().get_min_size())
    , p(new priv(this, main_frame))
{
    // Initialization performed in the private c-tor
    enable_wireframe(true);
    m_only_gcode = false;
}

bool Plater::Show(bool show)
{
    if (wxGetApp().mainframe)
        wxGetApp().mainframe->show_option(show);
    return wxPanel::Show(show);
}

bool Plater::is_project_dirty() const { return p->is_project_dirty(); }
bool Plater::is_presets_dirty() const { return p->is_presets_dirty(); }
void Plater::set_plater_dirty(bool is_dirty) { p->set_plater_dirty(is_dirty); }
void Plater::update_project_dirty_from_presets() { p->update_project_dirty_from_presets(); }
int  Plater::save_project_if_dirty(const wxString& reason) { return p->save_project_if_dirty(reason); }
void Plater::reset_project_dirty_after_save() { p->reset_project_dirty_after_save(); }
void Plater::reset_project_dirty_initial_presets() { p->reset_project_dirty_initial_presets(); }
#if ENABLE_PROJECT_DIRTY_STATE_DEBUG_WINDOW
void Plater::render_project_state_debug_window() const { p->render_project_state_debug_window(); }
#endif // ENABLE_PROJECT_DIRTY_STATE_DEBUG_WINDOW

Sidebar&        Plater::sidebar()           { return *p->sidebar; }
const Model&    Plater::model() const       { return p->model; }
Model&          Plater::model()             { return p->model; }
const Print&    Plater::fff_print() const   { return p->fff_print; }
Print&          Plater::fff_print()         { return p->fff_print; }
const SLAPrint& Plater::sla_print() const   { return p->sla_print; }
SLAPrint&       Plater::sla_print()         { return p->sla_print; }

int Plater::new_project(bool skip_confirm, bool silent, const wxString& project_name)
{
    bool transfer_preset_changes = false;
    // BBS: save confirm
    auto check = [&transfer_preset_changes](bool yes_or_no) {
        wxString header = _L("Some presets are modified.") + "\n" +
            (yes_or_no ? _L("You can keep the modified presets to the new project or discard them") :
                _L("You can keep the modified presets to the new project, discard or save changes as new presets."));
        int act_buttons = ActionButtons::KEEP | ActionButtons::REMEMBER_CHOISE;
        if (!yes_or_no)
            act_buttons |= ActionButtons::SAVE;
        return wxGetApp().check_and_keep_current_preset_changes(_L("Creating a new project"), header, act_buttons, &transfer_preset_changes);
    };
    int result;
    if (!skip_confirm && (result = close_with_confirm(check)) == wxID_CANCEL)
        return wxID_CANCEL;

    m_only_gcode = false;
    m_exported_file = false;
    m_loading_project = false;
    get_notification_manager()->bbl_close_plateinfo_notification();
    get_notification_manager()->bbl_close_preview_only_notification();
    get_notification_manager()->bbl_close_3mf_warn_notification();
    get_notification_manager()->close_notification_of_type(NotificationType::PlaterError);
    get_notification_manager()->close_notification_of_type(NotificationType::PlaterWarning);
    get_notification_manager()->close_notification_of_type(NotificationType::SlicingError);
    get_notification_manager()->close_notification_of_type(NotificationType::SlicingSeriousWarning);
    get_notification_manager()->close_notification_of_type(NotificationType::SlicingWarning);

    if (!silent)
        wxGetApp().mainframe->select_tab(MainFrame::tp3DEditor);

    //get_partplate_list().reinit();
    //get_partplate_list().update_slice_context_to_current_plate(p->background_process);
    //p->preview->update_gcode_result(p->partplate_list.get_current_slice_result());
    reset(transfer_preset_changes);
    reset_project_dirty_after_save();
    reset_project_dirty_initial_presets();
    wxGetApp().update_saved_preset_from_current_preset();
    update_project_dirty_from_presets();

    //reset project
    p->project.reset();
    //set project name
    if (project_name.empty())
        p->set_project_name(_L("Untitled"));
    else
        p->set_project_name(project_name);

    Plater::TakeSnapshot snapshot(this, "New Project", UndoRedo::SnapshotType::ProjectSeparator);

    Model m;
    model().load_from(m); // new id avoid same path name

    //select first plate
    get_partplate_list().select_plate(0);
    SimpleEvent event(EVT_GLCANVAS_PLATE_SELECT);
    p->on_plate_selected(event);

    p->load_auxiliary_files();
    wxGetApp().app_config->update_last_backup_dir(model().get_backup_path());

    // BBS set default view and zoom
    p->select_view_3D("3D");
    p->select_view("topfront");
    p->camera.requires_zoom_to_bed = true;
    enable_sidebar(!m_only_gcode);

    up_to_date(true, false);
    up_to_date(true, true);
    return wxID_YES;
}

LoadType determine_load_type(std::string filename, std::string override_setting = "");

// BBS: FIXME, missing resotre logic
void Plater::load_project(wxString const& filename2,
    wxString const& originfile)
{
    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << "filename is: " << filename2 << "and originfile is: " << originfile;
    BOOST_LOG_TRIVIAL(info) << __FUNCTION__;
    auto filename = filename2;
    auto check = [&filename, this] (bool yes_or_no) {
        if (!yes_or_no && !wxGetApp().check_and_save_current_preset_changes(_L("Load project"),
                _L("Some presets are modified.")))
            return false;
        if (filename.empty()) {
            // Ask user for a project file name.
            wxGetApp().load_project(this, filename);
        }
        return !filename.empty();
    };

    // BSS: save project, force close
    int result;
    if ((result = close_with_confirm(check)) == wxID_CANCEL) {
        return;
    }

    // BBS
    if (m_loading_project) {
        //some error cases happens
        //return directly
        BOOST_LOG_TRIVIAL(warning) << __FUNCTION__ << boost::format(": current loading other project, return directly");
        return;
    }
    else
        m_loading_project = true;

    m_only_gcode = false;
    m_exported_file = false;
    get_notification_manager()->bbl_close_plateinfo_notification();
    get_notification_manager()->bbl_close_preview_only_notification();
    get_notification_manager()->bbl_close_3mf_warn_notification();
    get_notification_manager()->close_notification_of_type(NotificationType::PlaterError);
    get_notification_manager()->close_notification_of_type(NotificationType::PlaterWarning);
    get_notification_manager()->close_notification_of_type(NotificationType::SlicingError);
    get_notification_manager()->close_notification_of_type(NotificationType::SlicingSeriousWarning);
    get_notification_manager()->close_notification_of_type(NotificationType::SlicingWarning);

    auto path     = into_path(filename);

    auto strategy = LoadStrategy::LoadModel | LoadStrategy::LoadConfig;
    if (originfile == "<silence>") {
        strategy = strategy | LoadStrategy::Silence;
    } else if (originfile == "<loadall>") {
        // Do nothing
    } else if (originfile != "-") {
        strategy = strategy | LoadStrategy::Restore;
    } else {
        switch (determine_load_type(filename.ToStdString())) {
            case LoadType::OpenProject: break; // Do nothing
            case LoadType::LoadGeometry:; strategy = LoadStrategy::LoadModel; break;
            default: return; // User cancelled
        }
    }
    bool load_restore = strategy & LoadStrategy::Restore;

    // Take the Undo / Redo snapshot.
    reset();

    Plater::TakeSnapshot snapshot(this, "Load Project", UndoRedo::SnapshotType::ProjectSeparator);

    std::vector<fs::path> input_paths;
    input_paths.push_back(path);
    if (strategy & LoadStrategy::Restore)
        input_paths.push_back(into_u8(originfile));

    std::vector<size_t> res = load_files(input_paths, strategy);

    reset_project_dirty_initial_presets();
    update_project_dirty_from_presets();
    wxGetApp().preset_bundle->export_selections(*wxGetApp().app_config);

    // if res is empty no data has been loaded
    if (!res.empty() && (load_restore || !(strategy & LoadStrategy::Silence))) {
        BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << __LINE__ << " call set_project_filename: " << (load_restore ? originfile : filename);
        p->set_project_filename(load_restore ? originfile : filename);
        if (load_restore && originfile.IsEmpty()) {
        p->set_project_name(_L("Untitled"));
        }

    } else {
        if (using_exported_file()) {
            BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << __LINE__ << " using ecported set project filename: " << filename;
            p->set_project_filename(filename);
        }

    }

    // BBS set default 3D view and direction after loading project
    //p->select_view_3D("3D");
    if (!m_exported_file) {
        p->select_view("topfront");
        p->camera.requires_zoom_to_plate = REQUIRES_ZOOM_TO_ALL_PLATE;
        wxGetApp().mainframe->select_tab(MainFrame::tp3DEditor);
    }
    else {
        p->partplate_list.select_plate_view();
    }

    enable_sidebar(!m_only_gcode);

    wxGetApp().app_config->update_last_backup_dir(model().get_backup_path());
    if (load_restore && !originfile.empty()) {
        wxGetApp().app_config->update_skein_dir(into_path(originfile).parent_path().string());
        wxGetApp().app_config->update_config_dir(into_path(originfile).parent_path().string());
    }

    if (!load_restore)
        up_to_date(true, false);
    else
        p->dirty_state.update_from_undo_redo_stack(true);
    up_to_date(true, true);

    wxGetApp().params_panel()->switch_to_object_if_has_object_configs();

    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << __LINE__ << " load project done";
    m_loading_project = false;
}

// BBS: save logic
int Plater::save_project(bool saveAs)
{
    //if (up_to_date(false, false)) // should we always save
    //    return;
    auto filename = get_project_filename(".3mf");
    if (!saveAs && filename.IsEmpty())
        saveAs = true;
    if (saveAs)
        filename = p->get_export_file(FT_3MF);
    if (filename.empty())
        return wxID_NO;
    if (filename == "<cancel>")
        return wxID_CANCEL;

    //BBS export 3mf without gcode
    if (export_3mf(into_path(filename), SaveStrategy::SplitModel | SaveStrategy::ShareMesh | SaveStrategy::FullPathSources) < 0) {
        MessageDialog(this, _L("Failed to save the project.\nPlease check whether the folder exists online or if other programs open the project file."),
            _L("Save project"), wxOK | wxICON_WARNING).ShowModal();
        return wxID_CANCEL;
    }

    Slic3r::remove_backup(model(), false);

    p->set_project_filename(filename);
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << __LINE__ << " call set_project_filename: " << filename;

    up_to_date(true, false);
    up_to_date(true, true);

    wxGetApp().update_saved_preset_from_current_preset();
    reset_project_dirty_after_save();
    try {
        json j;
        boost::uintmax_t size = boost::filesystem::file_size(into_path(filename));
        j["file_size"] = size;
        j["file_name"] = std::string(filename.mb_str());

        NetworkAgent* agent = wxGetApp().getAgent();
    }
    catch (...) {}

    update_title_dirty_status();
    return wxID_YES;
}

//BBS import model by model id
void Plater::import_model_id(wxString download_info)
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << __LINE__ << " download info: " << download_info;

    wxString download_origin_url = download_info;
    wxString download_url;
    wxString filename;
    wxString separator = "&name=";

    try
    {
        size_t namePos = download_info.Find(separator);
        if (namePos != wxString::npos) {
            download_url = download_info.Mid(0, namePos);
            filename = download_info.Mid(namePos + separator.Length());

        }
        else {
            fs::path download_path = fs::path(download_origin_url.wx_str());
            download_url = download_origin_url;
            filename = download_path.filename().string();
        }

    }
    catch (const std::exception&)
    {
        //wxString sError = error.what();
    }

    bool download_ok = false;
    int retry_count = 0;
    const int max_retries = 3;

    /* jump to 3D eidtor */
    wxGetApp().mainframe->select_tab((size_t)MainFrame::TabPosition::tp3DEditor);

    /* prepare progress dialog */
    bool cont = true;
    bool cont_dlg = true;
    bool cancel = false;
    wxString msg;
    wxString dlg_title = _L("Importing Model");

    int percent = 0;
    ProgressDialog dlg(dlg_title,
        wxString(' ', 100) + "\n\n\n\n",
        100,    // range
        this,   // parent
        wxPD_CAN_ABORT |
        wxPD_APP_MODAL |
        wxPD_AUTO_HIDE |
        wxPD_SMOOTH);

    boost::filesystem::path target_path;

    //reset params
    p->project.reset();

    /* prepare project and profile */
    boost::thread import_thread = Slic3r::create_thread([&percent, &cont, &cancel, &retry_count, max_retries, &msg, &target_path, &download_ok, download_url, &filename] {

        // Orca: NetworkAgent is not needed and only prevents this from running
//        NetworkAgent* m_agent = Slic3r::GUI::wxGetApp().getAgent();
//        if (!m_agent) return;

        int res = 0;
        std::string http_body;

        msg = _L("prepare 3mf file...");

        //gets the number of files with the same name
        std::vector<wxString>   vecFiles;
        bool                    is_already_exist = false;


        target_path = fs::path(wxGetApp().app_config->get("download_path"));

        try
        {
            vecFiles.clear();
            wxString extension = fs::path(filename.wx_str()).extension().c_str();


            //check file suffix
            if (!extension.Contains(".3mf")) {
                msg = _L("Download failed, unknown file format.");
                return;
            }

            auto name = filename.substr(0, filename.length() - extension.length() - 1);

            for (const auto& iter : boost::filesystem::directory_iterator(target_path))
            {
                if (boost::filesystem::is_directory(iter.path()))
                    continue;

                wxString sFile = iter.path().filename().string().c_str();
                if (strstr(sFile.c_str(), name.c_str()) != NULL) {
                    vecFiles.push_back(sFile);
                }

                if (sFile == filename) is_already_exist = true;
            }
        }
        catch (const std::exception&)
        {
            //wxString sError = error.what();
        }

        //update filename
        if (is_already_exist && vecFiles.size() >= 1) {
            wxString extension = fs::path(filename.wx_str()).extension().c_str();
            wxString name = filename.substr(0, filename.length() - extension.length());
            filename = wxString::Format("%s(%d)%s", name, vecFiles.size() + 1, extension).ToStdString();
        }


        msg = _L("downloading project...");

        //target_path = wxStandardPaths::Get().GetTempDir().utf8_str().data();


        //target_path = wxGetApp().get_local_models_path().c_str();
        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        std::string unique = to_string(uuid).substr(0, 6);

        if (filename.empty()) {
            filename = "untitled.3mf";
        }

        //target_path /= (boost::format("%1%_%2%.3mf") % filename % unique).str();
        target_path /= fs::path(filename.wc_str());

        fs::path tmp_path = target_path;
        tmp_path += format(".%1%", ".download");

        auto filesize = 0;
        bool size_limit = false;
        auto http = Http::get(download_url.ToStdString());

        while (cont && retry_count < max_retries) {
            retry_count++;
            http.on_progress([&percent, &cont, &msg, &filesize, &size_limit](Http::Progress progress, bool& cancel) {

                    if (!cont) cancel = true;
                    if (progress.dltotal != 0) {

                        if (filesize == 0) {
                            filesize = progress.dltotal;
                            double megabytes = static_cast<double>(progress.dltotal) / (1024 * 1024);
                            //The maximum size of a 3mf file is 500mb
                            if (megabytes > 500) {
                                cont = false;
                                size_limit = true;
                            }
                        }
                        percent = progress.dlnow * 100 / progress.dltotal;
                    }

                    if (size_limit) {
                        msg = _L("Download failed, File size exception.");
                    }
                    else {
                        msg = wxString::Format(_L("Project downloaded %d%%"), percent);
                    }
                })
                .on_error([&msg, &cont, &retry_count, max_retries](std::string body, std::string error, unsigned http_status) {
                    (void)body;
                    BOOST_LOG_TRIVIAL(error) << format("Error getting: `%1%`: HTTP %2%, %3%",
                        body,
                        http_status,
                        error);

                    if (retry_count == max_retries) {
                        msg = _L("Importing to Snapmaker Orca failed. Please download the file and manually import it.");
                        cont = false;
                    }
                })
                .on_complete([&cont, &download_ok, tmp_path, target_path](std::string body, unsigned /* http_status */) {
                        fs::fstream file(tmp_path, std::ios::out | std::ios::binary | std::ios::trunc);
                        file.write(body.c_str(), body.size());
                        file.close();
                        fs::rename(tmp_path, target_path);
                        cont = false;
                        download_ok = true;
                }).perform_sync();

                // for break while
                //cont = false;
        }

    });

    while (cont && cont_dlg) {
        wxMilliSleep(50);
        cont_dlg = dlg.Update(percent, msg);
        if (!cont_dlg) {
            cont = cont_dlg;
            cancel = true;
        }

        if (download_ok)
            break;
    }

    if (import_thread.joinable())
        import_thread.join();

    dlg.Hide();
    dlg.Close();
    if (download_ok) {
        BOOST_LOG_TRIVIAL(trace) << "import_model_id: target_path = " << target_path.string();
        /* load project */
        // Orca: If download is a zip file, treat it as if file has been drag and dropped on the plater
        if (target_path.extension() == ".zip")
            this->load_files(wxArrayString(1, target_path.string()));
        else
            this->load_project(target_path.wstring());
        /*BBS set project info after load project, project info is reset in load project */
        //p->project.project_model_id = model_id;
        //p->project.project_design_id = design_id;
        AppConfig* config = wxGetApp().app_config;
        if (config) {
            p->project.project_country_code = config->get_country_code();
        }

        // show save new project
        p->set_project_filename(target_path.wstring());
        p->notification_manager->push_import_finished_notification(target_path.string(), target_path.parent_path().string(), false);
    }
    else {
        if (!msg.empty()) {
            MessageDialog msg_wingow(nullptr, msg, wxEmptyString, wxICON_WARNING | wxOK);
            msg_wingow.SetSize(wxSize(FromDIP(480), -1));
            msg_wingow.ShowModal();
        }
        return;
    }
}
//BBS download project by project id
void Plater::download_project(const wxString& project_id)
{
    return;
}

void Plater::request_model_download(wxString url)
{
    wxCommandEvent* event = new wxCommandEvent(EVT_IMPORT_MODEL_ID);
    event->SetString(url);
    wxQueueEvent(this, event);
}

void Plater::request_download_project(std::string project_id)
{
    wxCommandEvent* event = new wxCommandEvent(EVT_DOWNLOAD_PROJECT);
    event->SetString(project_id);
    wxQueueEvent(this, event);
}

// BBS: save logic
bool Plater::up_to_date(bool saved, bool backup)
{
    if (saved) {
        Slic3r::clear_other_changes(backup);
        return p->up_to_date(saved, backup);
    }
    return p->model.objects.empty() || (p->up_to_date(saved, backup) &&
                                        !Slic3r::has_other_changes(backup));
}

void Plater::add_model(bool imperial_units, std::string fname)
{
    wxArrayString input_files;

    std::vector<fs::path> paths;
    if (fname.empty()) {
        wxGetApp().import_model(this, input_files);
        if (input_files.empty())
            return;

        for (const auto& file : input_files)
            paths.emplace_back(into_path(file));
    }
    else {
        paths.emplace_back(fname);
    }

    std::string snapshot_label;
    assert(! paths.empty());
    if (paths.size() == 1) {
        snapshot_label = "Import Object";
        snapshot_label += ": ";
        snapshot_label += encode_path(paths.front().filename().string().c_str());
    } else {
        snapshot_label = "Import Objects";
        snapshot_label += ": ";
        snapshot_label += paths.front().filename().string().c_str();
        for (size_t i = 1; i < paths.size(); ++ i) {
            snapshot_label += ", ";
            snapshot_label += encode_path(paths[i].filename().string().c_str());
        }
    }

    Plater::TakeSnapshot snapshot(this, snapshot_label);

    // BBS: check file types
    auto loadfiles_type  = LoadFilesType::NoFile;
    auto amf_files_count = get_3mf_file_count(paths);

    if (paths.size() > 1 && amf_files_count < paths.size()) { loadfiles_type = LoadFilesType::Multiple3MFOther; }
    if (paths.size() > 1 && amf_files_count == paths.size()) { loadfiles_type = LoadFilesType::Multiple3MF; }
    if (paths.size() > 1 && amf_files_count == 0) { loadfiles_type = LoadFilesType::MultipleOther; }
    if (paths.size() == 1 && amf_files_count == 1) { loadfiles_type = LoadFilesType::Single3MF; };
    if (paths.size() == 1 && amf_files_count == 0) { loadfiles_type = LoadFilesType::SingleOther; };

    bool ask_multi = false;

    if (loadfiles_type == LoadFilesType::MultipleOther)
        ask_multi = true;

    auto strategy = LoadStrategy::LoadModel;
    if (imperial_units) strategy = strategy | LoadStrategy::ImperialUnits;
    if (!load_files(paths, strategy, ask_multi).empty()) {

        if (get_project_name() == _L("Untitled") && paths.size() > 0) {
            boost::filesystem::path full_path(paths[0].string());
            p->set_project_name(from_u8(full_path.stem().string()));
        }

        wxGetApp().mainframe->update_title();
    }
}

void Plater::calib_pa(const Calib_Params& params)
{
    const auto calib_pa_name = wxString::Format(L"Pressure Advance Test");
    new_project(false, false, calib_pa_name);
    wxGetApp().mainframe->select_tab(size_t(MainFrame::tp3DEditor));
    switch (params.mode) {
        case CalibMode::Calib_PA_Line:
            add_model(false, Slic3r::resources_dir() + "/calib/pressure_advance/pressure_advance_test.stl");
            break;
        case CalibMode::Calib_PA_Pattern:
            _calib_pa_pattern(params);
            break;
        case CalibMode::Calib_PA_Tower:
            _calib_pa_tower(params);
            break;
        default: break;
    }
    auto printer_config = &wxGetApp().preset_bundle->printers.get_edited_preset().config;
    printer_config->set_key_value("resonance_avoidance", new ConfigOptionBool{false});
    p->background_process.fff_print()->set_calib_params(params);
}

void Plater::_calib_pa_pattern(const Calib_Params& params)
{
    std::vector<double> speeds{params.speeds};
    std::vector<double> accels{params.accelerations};
    std::vector<size_t> object_idxs{};
    /* Set common parameters */
    auto printer_config = &wxGetApp().preset_bundle->printers.get_edited_preset().config;
    DynamicPrintConfig& print_config = wxGetApp().preset_bundle->prints.get_edited_preset().config;
    auto filament_config = &wxGetApp().preset_bundle->filaments.get_edited_preset().config;
    double nozzle_diameter = printer_config->option<ConfigOptionFloats>("nozzle_diameter")->get_at(0);
    filament_config->set_key_value("filament_retract_when_changing_layer", new ConfigOptionBoolsNullable{false});
    filament_config->set_key_value("filament_wipe", new ConfigOptionBoolsNullable{false});
    printer_config->set_key_value("wipe", new ConfigOptionBools{false});
    printer_config->set_key_value("retract_when_changing_layer", new ConfigOptionBools{false});
    printer_config->set_key_value("resonance_avoidance", new ConfigOptionBool{false});

    //Orca: find acceleration to use in the test
    auto accel = print_config.option<ConfigOptionFloat>("outer_wall_acceleration")->value; // get the outer wall acceleration
    if (accel == 0) // if outer wall accel isnt defined, fall back to inner wall accel
        accel = print_config.option<ConfigOptionFloat>("inner_wall_acceleration")->value;
    if (accel == 0) // if inner wall accel is not defined fall back to default accel
        accel = print_config.option<ConfigOptionFloat>("default_acceleration")->value;
    // Orca: Set all accelerations except first layer, as the first layer accel doesnt affect the PA test since accel
    // is set to the travel accel before printing the pattern.
    if (accels.empty()) {
        accels.assign({accel});
        const auto msg{_L("INFO:") + "\n" +
                       _L("No accelerations provided for calibration. Use default acceleration value ") + std::to_string(long(accel)) + wxString::FromUTF8("mm/s²")};
        get_notification_manager()->push_notification(msg.ToStdString());
    } else {
        // set max acceleration in case of batch mode to get correct test pattern size
        accel = *std::max_element(accels.begin(), accels.end());
    }
    print_config.set_key_value( "outer_wall_acceleration", new ConfigOptionFloat(accel));
    print_config.set_key_value( "print_sequence", new ConfigOptionEnum(PrintSequence::ByLayer));
    
    //Orca: find jerk value to use in the test
    if(print_config.option<ConfigOptionFloat>("default_jerk")->value > 0){ // we have set a jerk value
        auto jerk = print_config.option<ConfigOptionFloat>("outer_wall_jerk")->value; // get outer wall jerk
        if (jerk == 0) // if outer wall jerk is not defined, get inner wall jerk
            jerk = print_config.option<ConfigOptionFloat>("inner_wall_jerk")->value;
        if (jerk == 0) // if inner wall jerk is not defined, get the default jerk
            jerk = print_config.option<ConfigOptionFloat>("default_jerk")->value;
        
        //Orca: Set jerk values. Again first layer jerk should not matter as it is reset to the travel jerk before the
        // first PA pattern is printed.
        print_config.set_key_value( "default_jerk", new ConfigOptionFloat(jerk));
        print_config.set_key_value( "outer_wall_jerk", new ConfigOptionFloat(jerk));
        print_config.set_key_value( "inner_wall_jerk", new ConfigOptionFloat(jerk));
        print_config.set_key_value( "top_surface_jerk", new ConfigOptionFloat(jerk));
        print_config.set_key_value( "infill_jerk", new ConfigOptionFloat(jerk));
        print_config.set_key_value( "travel_jerk", new ConfigOptionFloat(jerk));
    }
    
    for (const auto& opt : SuggestedConfigCalibPAPattern().float_pairs) {
        print_config.set_key_value(
            opt.first,
            new ConfigOptionFloat(opt.second)
        );
    }

    for (const auto& opt : SuggestedConfigCalibPAPattern().nozzle_ratio_pairs) {
        print_config.set_key_value(
            opt.first,
            new ConfigOptionFloatOrPercent(nozzle_diameter * opt.second / 100, false)
        );
    }

    for (const auto& opt : SuggestedConfigCalibPAPattern().int_pairs) {
        print_config.set_key_value(
            opt.first,
            new ConfigOptionInt(opt.second)
        );
    }

    print_config.set_key_value(
        SuggestedConfigCalibPAPattern().brim_pair.first,
        new ConfigOptionEnum<BrimType>(SuggestedConfigCalibPAPattern().brim_pair.second)
    );

    // Orca: Set the outer wall speed to the optimal speed for the test, cap it with max volumetric speed
    if (speeds.empty()) {
        double speed = CalibPressureAdvance::find_optimal_PA_speed(
            wxGetApp().preset_bundle->full_config(),
            print_config.get_abs_value("line_width", nozzle_diameter),
            print_config.get_abs_value("layer_height"), 0);
        print_config.set_key_value("outer_wall_speed", new ConfigOptionFloat(speed));

        speeds.assign({speed});
        const auto msg{_L("INFO:") + "\n" +
                       _L("No speeds provided for calibration. Use default optimal speed ") + std::to_string(long(speed)) + "mm/s"};
        get_notification_manager()->push_notification(msg.ToStdString());
    } else if (speeds.size() == 1) {
        // If we have single value provided, set speed using global configuration.
        // per-object config is not set in this case
        print_config.set_key_value("outer_wall_speed", new ConfigOptionFloat(speeds.front()));
    }

    wxGetApp().get_tab(Preset::TYPE_PRINT)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_FILAMENT)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_PRINTER)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_PRINT)->reload_config();
    wxGetApp().get_tab(Preset::TYPE_FILAMENT)->reload_config();
    wxGetApp().get_tab(Preset::TYPE_PRINTER)->reload_config();

    const DynamicPrintConfig full_config = wxGetApp().preset_bundle->full_config();
    PresetBundle* preset_bundle = wxGetApp().preset_bundle;
    const bool is_bbl_machine = preset_bundle->is_bbl_vendor();
    auto cur_plate = get_partplate_list().get_plate(0);

    // add "handle" cube
    sidebar().obj_list()->load_generic_subobject("Cube", ModelVolumeType::INVALID);
    auto *cube = model().objects[0];

    CalibPressureAdvancePattern pa_pattern(
        params,
        full_config,
        is_bbl_machine,
        *cube,
        cur_plate->get_origin()
    );

    /* Having PA pattern configured, we could make a set of polygons resembling N test patterns.
     * We'll arrange this set of polygons, so we would know position of each test pattern and
     * could position test cubes later on
     *
     * We'll take advantage of already existing cube: scale it up to test pattern size to use
     * as a reference for objects arrangement. Polygon is slightly oversized to add spaces between patterns.
     * That arrangement will be used to place 'handle cubes' for each test. */
    auto cube_bb = cube->raw_bounding_box();
    cube->scale((pa_pattern.print_size_x() + 4) / cube_bb.size().x(),
                (pa_pattern.print_size_y() + 4) / cube_bb.size().y(),
                pa_pattern.max_layer_z() / cube_bb.size().z());

    arrangement::ArrangePolygons arranged_items;
    {
        arrangement::ArrangeParams ap;
        Points bedpts = arrangement::get_shrink_bedpts(&full_config, ap);

        for(size_t i = 0; i < speeds.size() * accels.size(); i++) {
            arrangement::ArrangePolygon p;
            cube->instances[0]->get_arrange_polygon(&p);
            p.bed_idx = 0;
            arranged_items.emplace_back(p);
        }

        arrangement::arrange(arranged_items, bedpts, ap);
    }

    /* scale cube back to the size of test pattern 'handle' */
    cube_bb = cube->raw_bounding_box();
    cube->scale(pa_pattern.handle_xy_size() / cube_bb.size().x(),
                pa_pattern.handle_xy_size() / cube_bb.size().y(),
                pa_pattern.max_layer_z() / cube_bb.size().z());

    /* Set speed and acceleration on per-object basis and arrange anchor object on the plates.
     * Test gcode will be genecated during plate slicing */
    for(size_t test_idx = 0; test_idx < arranged_items.size(); test_idx++) {
        const auto &ai = arranged_items[test_idx];
        size_t plate_idx = arranged_items[test_idx].bed_idx;
        auto tspd = speeds[test_idx % speeds.size()];
        auto tacc = accels[test_idx / speeds.size()];

        /* make an own copy of anchor cube for each test */
        auto obj = test_idx == 0 ? cube : model().add_object(*cube);
        auto obj_idx = std::distance(model().objects.begin(), std::find(model().objects.begin(), model().objects.end(), obj));
        obj->name.assign(std::string("pa_pattern_") + std::to_string(int(tspd)) + std::string("_") + std::to_string(int(tacc)));

        auto &obj_config = obj->config;
        if (speeds.size() > 1)
            obj_config.set_key_value("outer_wall_speed", new ConfigOptionFloat(tspd));
        if (accels.size() > 1)
            obj_config.set_key_value("outer_wall_acceleration", new ConfigOptionFloat(tacc));

        auto cur_plate = get_partplate_list().get_plate(plate_idx);
        if (!cur_plate) {
            plate_idx = get_partplate_list().create_plate();
            cur_plate = get_partplate_list().get_plate(plate_idx);
        }

        object_idxs.emplace_back(obj_idx);
        get_partplate_list().add_to_plate(obj_idx, 0, plate_idx);
        const Vec3d obj_offset{unscale<double>(ai.translation(X)),
                               unscale<double>(ai.translation(Y)),
                               0};
        obj->instances[0]->set_offset(cur_plate->get_origin() + obj_offset + pa_pattern.handle_pos_offset());
        obj->ensure_on_bed();

        if (obj_idx == 0)
            sidebar().obj_list()->update_name_for_items();
        else
            sidebar().obj_list()->add_object_to_list(obj_idx);
    }

    model().calib_pa_pattern = std::make_unique<CalibPressureAdvancePattern>(pa_pattern);
    changed_objects(object_idxs);
}

void Plater::_calib_pa_pattern_gen_gcode()
{
    if (!model().calib_pa_pattern)
        return;

    auto cur_plate = get_partplate_list().get_curr_plate();
    if (cur_plate->empty())
        return;

    /* Container to store custom g-codes genereted by the test generator.
     * We'll store gcode for all tests on a single plate here. Once the plate handling is done,
     * all the g-codes will be merged into a single one on per-layer basis */
    std::vector<CustomGCode::Info> mgc;
    PresetBundle *preset_bundle = wxGetApp().preset_bundle;

    /* iterate over all cubes on current plate and generate gcode for them */
    for (auto obj : cur_plate->get_objects_on_this_plate()) {
        auto gcode = model().calib_pa_pattern->generate_custom_gcodes(
                                preset_bundle->full_config(),
                                preset_bundle->is_bbl_vendor(),
                                *obj,
                                cur_plate->get_origin()
        );
        mgc.emplace_back(gcode);
    }

    // move first item into model custom gcode
    auto &pcgc = model().plates_custom_gcodes[get_partplate_list().get_curr_plate_index()];
    pcgc = std::move(mgc[0]);
    mgc.erase(mgc.begin());

    // concat layer gcodes for each test
    for (size_t i = 0; i < pcgc.gcodes.size(); i++) {
        for (auto &gc : mgc) {
            pcgc.gcodes[i].extra += gc.gcodes[i].extra;
        }
    }
}

void Plater::cut_horizontal(size_t obj_idx, size_t instance_idx, double z, ModelObjectCutAttributes attributes)
{
    wxCHECK_RET(obj_idx < p->model.objects.size(), "obj_idx out of bounds");
    auto *object = p->model.objects[obj_idx];

    wxCHECK_RET(instance_idx < object->instances.size(), "instance_idx out of bounds");

    if (! attributes.has(ModelObjectCutAttribute::KeepUpper) && ! attributes.has(ModelObjectCutAttribute::KeepLower))
        return;

    wxBusyCursor wait;

    const Vec3d instance_offset = object->instances[instance_idx]->get_offset();
    Cut         cut(object, instance_idx, Geometry::translation_transform(z * Vec3d::UnitZ() - instance_offset), attributes);
    const auto  new_objects = cut.perform_with_plane();

    apply_cut_object_to_model(obj_idx, new_objects);
}

void Plater::_calib_pa_tower(const Calib_Params& params) {
    add_model(false, Slic3r::resources_dir() + "/calib/pressure_advance/tower_with_seam.stl");

    auto& print_config = wxGetApp().preset_bundle->prints.get_edited_preset().config;
    auto printer_config = &wxGetApp().preset_bundle->printers.get_edited_preset().config;
    auto filament_config = &wxGetApp().preset_bundle->filaments.get_edited_preset().config;

    const double nozzle_diameter = printer_config->option<ConfigOptionFloats>("nozzle_diameter")->get_at(0);

    filament_config->set_key_value("slow_down_layer_time", new ConfigOptionFloats{ 1.0f });


    auto& obj_cfg = model().objects[0]->config;

    obj_cfg.set_key_value("alternate_extra_wall", new ConfigOptionBool(false));
    auto full_config = wxGetApp().preset_bundle->full_config();
    auto wall_speed = CalibPressureAdvance::find_optimal_PA_speed(
        full_config, full_config.get_abs_value("line_width", nozzle_diameter),
        full_config.get_abs_value("layer_height"), 0);
    obj_cfg.set_key_value("outer_wall_speed", new ConfigOptionFloat(wall_speed));
    obj_cfg.set_key_value("inner_wall_speed", new ConfigOptionFloat(wall_speed));
    obj_cfg.set_key_value("seam_position", new ConfigOptionEnum<SeamPosition>(spRear));
    obj_cfg.set_key_value("wall_loops", new ConfigOptionInt(2));
    obj_cfg.set_key_value("top_shell_layers", new ConfigOptionInt(0));
    obj_cfg.set_key_value("bottom_shell_layers", new ConfigOptionInt(0));
    obj_cfg.set_key_value("sparse_infill_density", new ConfigOptionPercent(0));
    obj_cfg.set_key_value("brim_type", new ConfigOptionEnum<BrimType>(btEar));
    obj_cfg.set_key_value("brim_object_gap", new ConfigOptionFloat(.0f));
    obj_cfg.set_key_value("brim_ears_max_angle", new ConfigOptionFloat(135.f));
    obj_cfg.set_key_value("brim_width", new ConfigOptionFloat(6.f));
    obj_cfg.set_key_value("seam_slope_type", new ConfigOptionEnum<SeamScarfType>(SeamScarfType::None));
    print_config.set_key_value("max_volumetric_extrusion_rate_slope", new ConfigOptionFloat(0));

    changed_objects({ 0 });
    wxGetApp().get_tab(Preset::TYPE_PRINT)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_FILAMENT)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_PRINTER)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_PRINT)->reload_config();
    wxGetApp().get_tab(Preset::TYPE_FILAMENT)->reload_config();
    wxGetApp().get_tab(Preset::TYPE_PRINTER)->reload_config();

    auto new_height = std::ceil((params.end - params.start) / params.step) + 1;
    auto obj_bb = model().objects[0]->bounding_box_exact();
    if (new_height < obj_bb.size().z()) {
        cut_horizontal(0, 0, new_height, ModelObjectCutAttribute::KeepLower);
    }

    _calib_pa_select_added_objects();
}

void Plater::_calib_pa_select_added_objects() {
    // update printable state for new volumes on canvas3D
    wxGetApp().plater()->canvas3D()->update_instance_printable_state_for_objects({0});

    Selection& selection = p->view3D->get_canvas3d()->get_selection();
    selection.clear();
    selection.add_object(0, false);

    // BBS: update object list selection
    p->sidebar->obj_list()->update_selections();
    selection.notify_instance_update(-1, -1);
    if (p->view3D->get_canvas3d()->get_gizmos_manager().is_enabled()) {
        // this is required because the selected object changed and the flatten on face an sla support gizmos need to be updated accordingly
        p->view3D->get_canvas3d()->update_gizmos_on_off_state();
    }
}

// Adjust settings for flowrate calibration
// For linear mode, pass 1 means normal version while pass 2 mean "for perfectionists" version
void adjust_settings_for_flowrate_calib(ModelObjectPtrs& objects, bool linear, int pass)
{
    auto print_config = &wxGetApp().preset_bundle->prints.get_edited_preset().config;
    auto printerConfig = &wxGetApp().preset_bundle->printers.get_edited_preset().config;
    auto filament_config = &wxGetApp().preset_bundle->filaments.get_edited_preset().config;

    /// --- scale ---
    // model is created for a 0.4 nozzle, scale z with nozzle size.
    const ConfigOptionFloats* nozzle_diameter_config = printerConfig->option<ConfigOptionFloats>("nozzle_diameter");
    assert(nozzle_diameter_config->values.size() > 0);
    float nozzle_diameter = nozzle_diameter_config->values[0];
    float xyScale = nozzle_diameter / 0.6;
    //scale z to have 10 layers
    // 2 bottom, 5 top, 3 sparse infill
    double first_layer_height = print_config->option<ConfigOptionFloat>("initial_layer_print_height")->value;
    double layer_height = nozzle_diameter / 2.0; // prefer 0.2 layer height for 0.4 nozzle
    first_layer_height = std::max(first_layer_height, layer_height);

    const auto canvas    = wxGetApp().plater()->canvas3D();
    auto&      selection = canvas->get_selection();
    selection.setup_cache();
    TransformationType transformation_type;
    transformation_type.set_relative();
    float zscale = (first_layer_height + 9 * layer_height) / 2;
    // only enlarge
    if (xyScale > 1.2) {
        selection.scale({xyScale, xyScale, zscale}, transformation_type);
    } else {
        selection.scale({1, 1, zscale}, transformation_type);
    }
    canvas->do_scale("");

    auto cur_flowrate = filament_config->option<ConfigOptionFloats>("filament_flow_ratio")->get_at(0);
    Flow infill_flow = Flow(nozzle_diameter * 1.2f, layer_height, nozzle_diameter);
    double filament_max_volumetric_speed = filament_config->option<ConfigOptionFloats>("filament_max_volumetric_speed")->get_at(0);
    double max_infill_speed;
    if (linear)
        max_infill_speed = filament_max_volumetric_speed /
                           (infill_flow.mm3_per_mm() * (cur_flowrate + (pass == 2 ? 0.035 : 0.05)) / cur_flowrate);
    else
        max_infill_speed = filament_max_volumetric_speed / (infill_flow.mm3_per_mm() * (pass == 1 ? 1.2 : 1));
    double internal_solid_speed = std::floor(std::min(print_config->opt_float("internal_solid_infill_speed"), max_infill_speed));
    double top_surface_speed = std::floor(std::min(print_config->opt_float("top_surface_speed"), max_infill_speed));

    // adjust parameters
    for (auto _obj : objects) {
        _obj->ensure_on_bed();
        _obj->config.set_key_value("wall_loops", new ConfigOptionInt(1));
        _obj->config.set_key_value("only_one_wall_top", new ConfigOptionBool(true));
        _obj->config.set_key_value("thick_internal_bridges", new ConfigOptionBool(false));
        _obj->config.set_key_value("enable_extra_bridge_layer", new ConfigOptionEnum<EnableExtraBridgeLayer>(eblDisabled));
        _obj->config.set_key_value("internal_bridge_density", new ConfigOptionPercent(100));
        _obj->config.set_key_value("sparse_infill_density", new ConfigOptionPercent(35));
        _obj->config.set_key_value("min_width_top_surface", new ConfigOptionFloatOrPercent(100,true));
        _obj->config.set_key_value("bottom_shell_layers", new ConfigOptionInt(2));
        _obj->config.set_key_value("top_shell_layers", new ConfigOptionInt(5));
        _obj->config.set_key_value("top_shell_thickness", new ConfigOptionFloat(0));
        _obj->config.set_key_value("bottom_shell_thickness", new ConfigOptionFloat(0));
        _obj->config.set_key_value("detect_thin_wall", new ConfigOptionBool(true));
        _obj->config.set_key_value("filter_out_gap_fill", new ConfigOptionFloat(0));
        _obj->config.set_key_value("sparse_infill_pattern", new ConfigOptionEnum<InfillPattern>(ipRectilinear));
        _obj->config.set_key_value("top_surface_line_width", new ConfigOptionFloatOrPercent(nozzle_diameter * 1.2f, false));
        _obj->config.set_key_value("internal_solid_infill_line_width", new ConfigOptionFloatOrPercent(nozzle_diameter * 1.2f, false));
        _obj->config.set_key_value("top_surface_pattern", new ConfigOptionEnum<InfillPattern>(ipArchimedeanChords));
        _obj->config.set_key_value("top_solid_infill_flow_ratio", new ConfigOptionFloat(1.0f));
        _obj->config.set_key_value("infill_direction", new ConfigOptionFloat(45));
        _obj->config.set_key_value("solid_infill_direction", new ConfigOptionFloat(135));
        _obj->config.set_key_value("align_infill_direction_to_model", new ConfigOptionBool(true));
        _obj->config.set_key_value("ironing_type", new ConfigOptionEnum<IroningType>(IroningType::NoIroning));
        _obj->config.set_key_value("internal_solid_infill_speed", new ConfigOptionFloat(internal_solid_speed));
        _obj->config.set_key_value("top_surface_speed", new ConfigOptionFloat(top_surface_speed));
        _obj->config.set_key_value("seam_slope_type", new ConfigOptionEnum<SeamScarfType>(SeamScarfType::None));
        _obj->config.set_key_value("gap_fill_target", new ConfigOptionEnum<GapFillTarget>(GapFillTarget::gftNowhere));
        print_config->set_key_value("max_volumetric_extrusion_rate_slope", new ConfigOptionFloat(0));
        _obj->config.set_key_value("calib_flowrate_topinfill_special_order", new ConfigOptionBool(true));

        // extract flowrate from name, filename format: flowrate_xxx
        std::string obj_name = _obj->name;
        assert(obj_name.length() > 9);
        obj_name = obj_name.substr(9);
        if (obj_name[0] == 'm')
            obj_name[0] = '-';
        // Orca: force set locale to C to avoid parsing error
        const std::string _loc = std::setlocale(LC_NUMERIC, nullptr);
        std::setlocale(LC_NUMERIC,"C");
        auto              modifier  = 1.0f;
        try {
            modifier = stof(obj_name);
        } catch (...) {
        }
        // restore locale
        std::setlocale(LC_NUMERIC, _loc.c_str());

        if(linear)
            _obj->config.set_key_value("print_flow_ratio", new ConfigOptionFloat((cur_flowrate + modifier)/cur_flowrate));
        else
            _obj->config.set_key_value("print_flow_ratio", new ConfigOptionFloat(1.0f + modifier/100.f));

    }

    print_config->set_key_value("layer_height", new ConfigOptionFloat(layer_height));
    print_config->set_key_value("alternate_extra_wall", new ConfigOptionBool(false));
    print_config->set_key_value("initial_layer_print_height", new ConfigOptionFloat(first_layer_height));
    print_config->set_key_value("reduce_crossing_wall", new ConfigOptionBool(true));


    wxGetApp().get_tab(Preset::TYPE_PRINT)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_FILAMENT)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_PRINTER)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_PRINT)->reload_config();
    wxGetApp().get_tab(Preset::TYPE_FILAMENT)->reload_config();
    wxGetApp().get_tab(Preset::TYPE_PRINTER)->reload_config();
}

void Plater::calib_flowrate(bool is_linear, int pass) {
    if (pass != 1 && pass != 2)
        return;
    wxString calib_name;
    if (is_linear) {
        calib_name = L"Orca YOLO Flow Calibration";
        if (pass == 2)
            calib_name += L" - Perfectionist version";
    } else
        calib_name = wxString::Format(L"Flowrate Test - Pass%d", pass);

    if (new_project(false, false, calib_name) == wxID_CANCEL)
        return;

    wxGetApp().mainframe->select_tab(size_t(MainFrame::tp3DEditor));

    if (is_linear) {
        if (pass == 1)
            add_model(false,
                      (boost::filesystem::path(Slic3r::resources_dir()) / "calib" / "filament_flow" / "Orca-LinearFlow.3mf").string());
        else
            add_model(false,
                      (boost::filesystem::path(Slic3r::resources_dir()) / "calib" / "filament_flow" / "Orca-LinearFlow_fine.3mf").string());
    } else {
        if (pass == 1)
            add_model(false,
                      (boost::filesystem::path(Slic3r::resources_dir()) / "calib" / "filament_flow" / "flowrate-test-pass1.3mf").string());
        else
            add_model(false,
                      (boost::filesystem::path(Slic3r::resources_dir()) / "calib" / "filament_flow" / "flowrate-test-pass2.3mf").string());
    }

    adjust_settings_for_flowrate_calib(model().objects, is_linear, pass);
    wxGetApp().get_tab(Preset::TYPE_PRINTER)->reload_config();
    auto printer_config = &wxGetApp().preset_bundle->printers.get_edited_preset().config;
    printer_config->set_key_value("resonance_avoidance", new ConfigOptionBool{false});

    // Refresh object after scaling
    const std::vector<size_t> object_idx(boost::counting_iterator<size_t>(0), boost::counting_iterator<size_t>(model().objects.size()));
    changed_objects(object_idx);
}


void Plater::calib_temp(const Calib_Params& params) {
    const auto calib_temp_name = wxString::Format(L"Nozzle temperature test");
    new_project(false, false, calib_temp_name);
    wxGetApp().mainframe->select_tab(size_t(MainFrame::tp3DEditor));
    if (params.mode != CalibMode::Calib_Temp_Tower)
        return;
    
    add_model(false, Slic3r::resources_dir() + "/calib/temperature_tower/temperature_tower.stl");
    auto printer_config = &wxGetApp().preset_bundle->printers.get_edited_preset().config;
    auto filament_config = &wxGetApp().preset_bundle->filaments.get_edited_preset().config;
    auto start_temp = lround(params.start);
    printer_config->set_key_value("resonance_avoidance", new ConfigOptionBool{false});
    filament_config->set_key_value("nozzle_temperature_initial_layer", new ConfigOptionInts(1,(int)start_temp));
    filament_config->set_key_value("nozzle_temperature", new ConfigOptionInts(1,(int)start_temp));
    model().objects[0]->config.set_key_value("brim_type", new ConfigOptionEnum<BrimType>(btOuterOnly));
    model().objects[0]->config.set_key_value("brim_width", new ConfigOptionFloat(5.0));
    model().objects[0]->config.set_key_value("brim_object_gap", new ConfigOptionFloat(0.0));
    model().objects[0]->config.set_key_value("alternate_extra_wall", new ConfigOptionBool(false));
    model().objects[0]->config.set_key_value("seam_slope_type", new ConfigOptionEnum<SeamScarfType>(SeamScarfType::None));

    changed_objects({ 0 });
    wxGetApp().get_tab(Preset::TYPE_PRINT)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_FILAMENT)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_PRINT)->reload_config();
    wxGetApp().get_tab(Preset::TYPE_FILAMENT)->reload_config();

    // cut upper
    auto obj_bb = model().objects[0]->bounding_box_exact();
    auto block_count = lround((350 - params.end) / 5 + 1);
    if(block_count > 0){
        // add EPSILON offset to avoid cutting at the exact location where the flat surface is
        auto new_height = block_count * 10.0 + EPSILON;
        if (new_height < obj_bb.size().z()) {
            cut_horizontal(0, 0, new_height, ModelObjectCutAttribute::KeepLower);
        }
    }
    
    // cut bottom
    obj_bb = model().objects[0]->bounding_box_exact();
    block_count = lround((350 - params.start) / 5);
    if(block_count > 0){
        auto new_height = block_count * 10.0 + EPSILON;
        if (new_height < obj_bb.size().z()) {
            cut_horizontal(0, 0, new_height, ModelObjectCutAttribute::KeepUpper);
        }
    }
    
    p->background_process.fff_print()->set_calib_params(params);
}

void Plater::calib_max_vol_speed(const Calib_Params& params)
{
    const auto calib_vol_speed_name = wxString::Format(L"Max volumetric speed test");
    new_project(false, false, calib_vol_speed_name);
    wxGetApp().mainframe->select_tab(size_t(MainFrame::tp3DEditor));
    if (params.mode != CalibMode::Calib_Vol_speed_Tower)
        return;

    add_model(false, Slic3r::resources_dir() + "/calib/volumetric_speed/SpeedTestStructure.step");

    auto print_config = &wxGetApp().preset_bundle->prints.get_edited_preset().config;
    auto filament_config = &wxGetApp().preset_bundle->filaments.get_edited_preset().config;
    auto printer_config = &wxGetApp().preset_bundle->printers.get_edited_preset().config;
    auto obj = model().objects[0];
    auto& obj_cfg = obj->config;

    auto bed_shape = printer_config->option<ConfigOptionPoints>("printable_area")->values;
    BoundingBoxf bed_ext = get_extents(bed_shape);
    auto scale_obj = (bed_ext.size().x() - 10) / obj->bounding_box_exact().size().x();
    if (scale_obj < 1.0)
        obj->scale(scale_obj, 1, 1);

    const ConfigOptionFloats* nozzle_diameter_config = printer_config->option<ConfigOptionFloats>("nozzle_diameter");
    assert(nozzle_diameter_config->values.size() > 0);
    double nozzle_diameter = nozzle_diameter_config->values[0];
    double line_width = nozzle_diameter * 1.75;
    double layer_height = nozzle_diameter * 0.8;

    auto max_lh = printer_config->option<ConfigOptionFloats>("max_layer_height");
    if (max_lh->values[0] < layer_height)
        max_lh->values[0] = { layer_height };

    filament_config->set_key_value("filament_max_volumetric_speed", new ConfigOptionFloats { 200 });
    filament_config->set_key_value("slow_down_layer_time", new ConfigOptionFloats{0.0});
    printer_config->set_key_value("resonance_avoidance", new ConfigOptionBool{false});
    obj_cfg.set_key_value("enable_overhang_speed", new ConfigOptionBool { false });
    obj_cfg.set_key_value("wall_loops", new ConfigOptionInt(1));
    obj_cfg.set_key_value("alternate_extra_wall", new ConfigOptionBool(false));
    obj_cfg.set_key_value("top_shell_layers", new ConfigOptionInt(0));
    obj_cfg.set_key_value("bottom_shell_layers", new ConfigOptionInt(0));
    obj_cfg.set_key_value("sparse_infill_density", new ConfigOptionPercent(0));
    obj_cfg.set_key_value("overhang_reverse", new ConfigOptionBool(false));
    obj_cfg.set_key_value("outer_wall_line_width", new ConfigOptionFloatOrPercent(line_width, false));
    obj_cfg.set_key_value("layer_height", new ConfigOptionFloat(layer_height));
    obj_cfg.set_key_value("brim_type", new ConfigOptionEnum<BrimType>(btOuterAndInner));
    obj_cfg.set_key_value("brim_width", new ConfigOptionFloat(5.0));
    obj_cfg.set_key_value("brim_object_gap", new ConfigOptionFloat(0.0));
    print_config->set_key_value("timelapse_type", new ConfigOptionEnum<TimelapseType>(tlTraditional));
    print_config->set_key_value("spiral_mode", new ConfigOptionBool(true));
    print_config->set_key_value("max_volumetric_extrusion_rate_slope", new ConfigOptionFloat(0));

    changed_objects({ 0 });
    wxGetApp().get_tab(Preset::TYPE_PRINT)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_FILAMENT)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_PRINTER)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_PRINT)->reload_config();
    wxGetApp().get_tab(Preset::TYPE_FILAMENT)->reload_config();
    wxGetApp().get_tab(Preset::TYPE_PRINTER)->reload_config();

    //  cut upper
    auto obj_bb = obj->bounding_box_exact();
    auto height = (params.end - params.start + 1) / params.step;
    if (height < obj_bb.size().z()) {
        cut_horizontal(0, 0, height, ModelObjectCutAttribute::KeepLower);
    }

    auto new_params = params;
    auto mm3_per_mm = Flow(line_width, layer_height, nozzle_diameter).mm3_per_mm() *
                      filament_config->option<ConfigOptionFloats>("filament_flow_ratio")->get_at(0);
    new_params.end = params.end / mm3_per_mm;
    new_params.start = params.start / mm3_per_mm;
    new_params.step = params.step / mm3_per_mm;


    p->background_process.fff_print()->set_calib_params(new_params);
}

void Plater::calib_retraction(const Calib_Params& params)
{
    const auto calib_retraction_name = wxString::Format(L"Retraction test");
    new_project(false, false, calib_retraction_name);
    wxGetApp().mainframe->select_tab(size_t(MainFrame::tp3DEditor));
    if (params.mode != CalibMode::Calib_Retraction_tower)
        return;

    add_model(false, Slic3r::resources_dir() + "/calib/retraction/retraction_tower.stl");

    auto print_config = &wxGetApp().preset_bundle->prints.get_edited_preset().config;
    auto filament_config = &wxGetApp().preset_bundle->filaments.get_edited_preset().config;
    auto printer_config = &wxGetApp().preset_bundle->printers.get_edited_preset().config;
    auto obj = model().objects[0];

    double layer_height = 0.2;

    auto max_lh = printer_config->option<ConfigOptionFloats>("max_layer_height");
    if (max_lh->values[0] < layer_height)
        max_lh->values[0] = { layer_height };

    printer_config->set_key_value("resonance_avoidance", new ConfigOptionBool{false});
    printer_config->set_key_value("use_firmware_retraction", new ConfigOptionBool(false));
    obj->config.set_key_value("wall_loops", new ConfigOptionInt(2));
    obj->config.set_key_value("top_shell_layers", new ConfigOptionInt(0));
    obj->config.set_key_value("bottom_shell_layers", new ConfigOptionInt(3));
    obj->config.set_key_value("sparse_infill_density", new ConfigOptionPercent(0));
    print_config->set_key_value("initial_layer_print_height", new ConfigOptionFloat(layer_height));
    obj->config.set_key_value("layer_height", new ConfigOptionFloat(layer_height));
    obj->config.set_key_value("alternate_extra_wall", new ConfigOptionBool(false));

    changed_objects({ 0 });

    //  cut upper
    auto obj_bb = obj->bounding_box_exact();
    auto height = 1.0 + 0.4 + ((params.end - params.start)) / params.step;
    if (height < obj_bb.size().z()) {
        cut_horizontal(0, 0, height, ModelObjectCutAttribute::KeepLower);
    }

    p->background_process.fff_print()->set_calib_params(params);
}

void Plater::calib_VFA(const Calib_Params& params)
{
    const auto calib_vfa_name = wxString::Format(L"VFA test");
    new_project(false, false, calib_vfa_name);
    wxGetApp().mainframe->select_tab(size_t(MainFrame::tp3DEditor));
    if (params.mode != CalibMode::Calib_VFA_Tower)
        return;

    add_model(false, Slic3r::resources_dir() + "/calib/vfa/VFA.stl");
    auto print_config = &wxGetApp().preset_bundle->prints.get_edited_preset().config;
    auto filament_config = &wxGetApp().preset_bundle->filaments.get_edited_preset().config;
    auto printer_config  = &wxGetApp().preset_bundle->printers.get_edited_preset().config;
    printer_config->set_key_value("resonance_avoidance", new ConfigOptionBool{false});
    filament_config->set_key_value("slow_down_layer_time", new ConfigOptionFloats { 0.0 });
    print_config->set_key_value("enable_overhang_speed", new ConfigOptionBool { false });
    print_config->set_key_value("timelapse_type", new ConfigOptionEnum<TimelapseType>(tlTraditional));
    print_config->set_key_value("wall_loops", new ConfigOptionInt(1));
    print_config->set_key_value("alternate_extra_wall", new ConfigOptionBool(false));
    print_config->set_key_value("top_shell_layers", new ConfigOptionInt(0));
    print_config->set_key_value("bottom_shell_layers", new ConfigOptionInt(1));
    print_config->set_key_value("sparse_infill_density", new ConfigOptionPercent(0));
    print_config->set_key_value("overhang_reverse", new ConfigOptionBool(false));
    print_config->set_key_value("detect_thin_wall", new ConfigOptionBool(false));
    print_config->set_key_value("spiral_mode", new ConfigOptionBool(true));
    model().objects[0]->config.set_key_value("brim_type", new ConfigOptionEnum<BrimType>(btOuterOnly));
    model().objects[0]->config.set_key_value("brim_width", new ConfigOptionFloat(3.0));
    model().objects[0]->config.set_key_value("brim_object_gap", new ConfigOptionFloat(0.0));

    changed_objects({ 0 });
    wxGetApp().get_tab(Preset::TYPE_PRINT)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_FILAMENT)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_PRINT)->update_ui_from_settings();
    wxGetApp().get_tab(Preset::TYPE_FILAMENT)->update_ui_from_settings();

    // cut upper
    auto obj_bb = model().objects[0]->bounding_box_exact();
    auto height = 5 * ((params.end - params.start) / params.step + 1);
    if (height < obj_bb.size().z()) {
        cut_horizontal(0, 0, height, ModelObjectCutAttribute::KeepLower);
    }

    p->background_process.fff_print()->set_calib_params(params);
}

void Plater::calib_input_shaping_freq(const Calib_Params& params)
{
    const auto calib_input_shaping_name = wxString::Format(L"Input shaping Frequency test");
    new_project(false, false, calib_input_shaping_name);
    wxGetApp().mainframe->select_tab(size_t(MainFrame::tp3DEditor));
    if (params.mode != CalibMode::Calib_Input_shaping_freq)
        return;

    add_model(false, Slic3r::resources_dir() + (params.test_model < 1 ? "/calib/input_shaping/ringing_tower.stl" : "/calib/input_shaping/fast_tower_test.stl"));
    auto print_config = &wxGetApp().preset_bundle->prints.get_edited_preset().config;
    auto filament_config = &wxGetApp().preset_bundle->filaments.get_edited_preset().config;
    auto printer_config  = &wxGetApp().preset_bundle->printers.get_edited_preset().config;
    printer_config->set_key_value("machine_max_junction_deviation", new ConfigOptionFloats {0.3});
    printer_config->set_key_value("resonance_avoidance", new ConfigOptionBool{false});
    filament_config->set_key_value("slow_down_layer_time", new ConfigOptionFloats { 0.0 });
    filament_config->set_key_value("slow_down_min_speed", new ConfigOptionFloats { 0.0 });
    filament_config->set_key_value("slow_down_for_layer_cooling", new ConfigOptionBools{false});
    filament_config->set_key_value("enable_pressure_advance", new ConfigOptionBools {true});
    filament_config->set_key_value("pressure_advance", new ConfigOptionFloats { 0.0 });
    filament_config->set_key_value("adaptive_pressure_advance", new ConfigOptionBools{false});
    print_config->set_key_value("layer_height", new ConfigOptionFloat(0.2));
    print_config->set_key_value("enable_overhang_speed", new ConfigOptionBool { false });
    print_config->set_key_value("timelapse_type", new ConfigOptionEnum<TimelapseType>(tlTraditional));
    print_config->set_key_value("wall_loops", new ConfigOptionInt(1));
    print_config->set_key_value("top_shell_layers", new ConfigOptionInt(0));
    print_config->set_key_value("bottom_shell_layers", new ConfigOptionInt(1));
    print_config->set_key_value("sparse_infill_density", new ConfigOptionPercent(0));
    print_config->set_key_value("detect_thin_wall", new ConfigOptionBool(false));
    print_config->set_key_value("spiral_mode", new ConfigOptionBool(true));
    print_config->set_key_value("spiral_mode_smooth", new ConfigOptionBool(false));
    print_config->set_key_value("bottom_surface_pattern", new ConfigOptionEnum<InfillPattern>(ipRectilinear));
    print_config->set_key_value("outer_wall_speed", new ConfigOptionFloat(200));
    print_config->set_key_value("default_acceleration", new ConfigOptionFloat(2000));
    print_config->set_key_value("outer_wall_acceleration", new ConfigOptionFloat(2000));
    print_config->set_key_value("default_junction_deviation", new ConfigOptionFloat(0.25));
    model().objects[0]->config.set_key_value("brim_type", new ConfigOptionEnum<BrimType>(btOuterOnly));
    model().objects[0]->config.set_key_value("brim_width", new ConfigOptionFloat(3.0));
    model().objects[0]->config.set_key_value("brim_object_gap", new ConfigOptionFloat(0.0));

    changed_objects({ 0 });
    wxGetApp().get_tab(Preset::TYPE_PRINT)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_FILAMENT)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_PRINT)->update_ui_from_settings();
    wxGetApp().get_tab(Preset::TYPE_FILAMENT)->update_ui_from_settings();

    p->background_process.fff_print()->set_calib_params(params);
}

void Plater::calib_input_shaping_damp(const Calib_Params& params)
{
    const auto calib_input_shaping_name = wxString::Format(L"Input shaping Damping test");
    new_project(false, false, calib_input_shaping_name);
    wxGetApp().mainframe->select_tab(size_t(MainFrame::tp3DEditor));
    if (params.mode != CalibMode::Calib_Input_shaping_damp)
        return;

    add_model(false, Slic3r::resources_dir() + (params.test_model < 1 ? "/calib/input_shaping/ringing_tower.stl" : "/calib/input_shaping/fast_tower_test.stl"));
    auto print_config = &wxGetApp().preset_bundle->prints.get_edited_preset().config;
    auto filament_config = &wxGetApp().preset_bundle->filaments.get_edited_preset().config;
    auto printer_config  = &wxGetApp().preset_bundle->printers.get_edited_preset().config;
    printer_config->set_key_value("machine_max_junction_deviation", new ConfigOptionFloats{0.3});
    printer_config->set_key_value("resonance_avoidance", new ConfigOptionBool{false});
    filament_config->set_key_value("slow_down_layer_time", new ConfigOptionFloats { 0.0 });
    filament_config->set_key_value("slow_down_min_speed", new ConfigOptionFloats { 0.0 });
    filament_config->set_key_value("slow_down_for_layer_cooling", new ConfigOptionBools{false});
    filament_config->set_key_value("enable_pressure_advance", new ConfigOptionBools {true});
    filament_config->set_key_value("pressure_advance", new ConfigOptionFloats { 0.0 });
    filament_config->set_key_value("adaptive_pressure_advance", new ConfigOptionBools{false});
    print_config->set_key_value("layer_height", new ConfigOptionFloat(0.2));
    print_config->set_key_value("enable_overhang_speed", new ConfigOptionBool{false});
    print_config->set_key_value("timelapse_type", new ConfigOptionEnum<TimelapseType>(tlTraditional));
    print_config->set_key_value("wall_loops", new ConfigOptionInt(1));
    print_config->set_key_value("top_shell_layers", new ConfigOptionInt(0));
    print_config->set_key_value("bottom_shell_layers", new ConfigOptionInt(1));
    print_config->set_key_value("sparse_infill_density", new ConfigOptionPercent(0));
    print_config->set_key_value("detect_thin_wall", new ConfigOptionBool(false));
    print_config->set_key_value("spiral_mode", new ConfigOptionBool(true));
    print_config->set_key_value("spiral_mode_smooth", new ConfigOptionBool(false));
    print_config->set_key_value("bottom_surface_pattern", new ConfigOptionEnum<InfillPattern>(ipRectilinear));
    print_config->set_key_value("outer_wall_speed", new ConfigOptionFloat(200));
    print_config->set_key_value("default_acceleration", new ConfigOptionFloat(2000));
    print_config->set_key_value("outer_wall_acceleration", new ConfigOptionFloat(2000));
    print_config->set_key_value("default_junction_deviation", new ConfigOptionFloat(0.25));
    model().objects[0]->config.set_key_value("brim_type", new ConfigOptionEnum<BrimType>(btOuterOnly));
    model().objects[0]->config.set_key_value("brim_width", new ConfigOptionFloat(3.0));
    model().objects[0]->config.set_key_value("brim_object_gap", new ConfigOptionFloat(0.0));

    changed_objects({ 0 });
    wxGetApp().get_tab(Preset::TYPE_PRINT)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_FILAMENT)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_PRINT)->update_ui_from_settings();
    wxGetApp().get_tab(Preset::TYPE_FILAMENT)->update_ui_from_settings();

    p->background_process.fff_print()->set_calib_params(params);
}

void Plater::calib_junction_deviation(const Calib_Params& params)
{
    const auto calib_junction_deviation = wxString::Format(L"Junction Deviation test");
    new_project(false, false, calib_junction_deviation);
    wxGetApp().mainframe->select_tab(size_t(MainFrame::tp3DEditor));
    if (params.mode != CalibMode::Calib_Junction_Deviation)
        return;

    add_model(false, Slic3r::resources_dir() + (params.test_model < 1 ? "/calib/input_shaping/ringing_tower.stl" : "/calib/input_shaping/fast_tower_test.stl"));
    auto print_config = &wxGetApp().preset_bundle->prints.get_edited_preset().config;
    auto filament_config = &wxGetApp().preset_bundle->filaments.get_edited_preset().config;
    auto printer_config  = &wxGetApp().preset_bundle->printers.get_edited_preset().config;
    printer_config->set_key_value("machine_max_junction_deviation", new ConfigOptionFloats{1.0});
    printer_config->set_key_value("resonance_avoidance", new ConfigOptionBool{false});
    filament_config->set_key_value("slow_down_layer_time", new ConfigOptionFloats { 0.0 });
    filament_config->set_key_value("slow_down_min_speed", new ConfigOptionFloats { 0.0 });
    filament_config->set_key_value("slow_down_for_layer_cooling", new ConfigOptionBools{false});
    filament_config->set_key_value("filament_max_volumetric_speed", new ConfigOptionFloats{200});
    filament_config->set_key_value("enable_pressure_advance", new ConfigOptionBools {true});
    filament_config->set_key_value("pressure_advance", new ConfigOptionFloats { 0.0 });
    filament_config->set_key_value("adaptive_pressure_advance", new ConfigOptionBools{false});
    print_config->set_key_value("layer_height", new ConfigOptionFloat(0.2));
    print_config->set_key_value("enable_overhang_speed", new ConfigOptionBool{false});
    print_config->set_key_value("timelapse_type", new ConfigOptionEnum<TimelapseType>(tlTraditional));
    print_config->set_key_value("wall_loops", new ConfigOptionInt(1));
    print_config->set_key_value("top_shell_layers", new ConfigOptionInt(0));
    print_config->set_key_value("bottom_shell_layers", new ConfigOptionInt(1));
    print_config->set_key_value("sparse_infill_density", new ConfigOptionPercent(0));
    print_config->set_key_value("detect_thin_wall", new ConfigOptionBool(false));
    print_config->set_key_value("spiral_mode", new ConfigOptionBool(true));
    print_config->set_key_value("spiral_mode_smooth", new ConfigOptionBool(false));
    print_config->set_key_value("bottom_surface_pattern", new ConfigOptionEnum<InfillPattern>(ipRectilinear));
    print_config->set_key_value("outer_wall_speed", new ConfigOptionFloat(200));
    print_config->set_key_value("default_acceleration", new ConfigOptionFloat(2000));
    print_config->set_key_value("outer_wall_acceleration", new ConfigOptionFloat(2000));
    print_config->set_key_value("default_junction_deviation", new ConfigOptionFloat(0.0));
    model().objects[0]->config.set_key_value("brim_type", new ConfigOptionEnum<BrimType>(btOuterOnly));
    model().objects[0]->config.set_key_value("brim_width", new ConfigOptionFloat(3.0));
    model().objects[0]->config.set_key_value("brim_object_gap", new ConfigOptionFloat(0.0));

    changed_objects({ 0 });
    wxGetApp().get_tab(Preset::TYPE_PRINT)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_FILAMENT)->update_dirty();
    wxGetApp().get_tab(Preset::TYPE_PRINT)->update_ui_from_settings();
    wxGetApp().get_tab(Preset::TYPE_FILAMENT)->update_ui_from_settings();
    
    p->background_process.fff_print()->set_calib_params(params);
}

BuildVolume_Type Plater::get_build_volume_type() const { return p->bed.get_build_volume_type(); }

void Plater::import_zip_archive()
{
    wxString input_file;
    wxGetApp().import_zip(this, input_file);
    if (input_file.empty())
        return;

    wxArrayString arr;
    arr.Add(input_file);
    load_files(arr);
}

void Plater::import_sl1_archive()
{
    auto &w = get_ui_job_worker();
    if (w.is_idle() && p->m_sla_import_dlg->ShowModal() == wxID_OK) {
        p->take_snapshot(_u8L("Import SLA archive"));
        replace_job(w, std::make_unique<SLAImportJob>(p->m_sla_import_dlg));
    }
}

void Plater::extract_config_from_project()
{
    wxString input_file;
    wxGetApp().load_project(this, input_file);

    if (! input_file.empty())
        load_files({ into_path(input_file) }, LoadStrategy::LoadConfig);
}

void Plater::load_gcode()
{
    // Ask user for a gcode file name.
    wxString input_file;
    wxGetApp().load_gcode(this, input_file);
    // And finally load the gcode file.
    load_gcode(input_file);
}

//BBS: remove GCodeViewer as seperate APP logic
void Plater::load_gcode(const wxString& filename)
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << __LINE__ << " entry and filename: " << filename;
    BOOST_LOG_TRIVIAL(info) << __FUNCTION__;
    if (! is_gcode_file(into_u8(filename))
        || (m_last_loaded_gcode == filename && m_only_gcode)
        )
        return;

    // Reject a missing / inaccessible file up front. Without this check the
    // code below would walk through process_file -> parse_file_raw_internal,
    // which used to crash on a NULL FILE* (now it just returns false), and
    // would otherwise surface the misleading "does not contain valid G-code"
    // message even when the real problem is that the file doesn't exist at all.
    if (!wxFileExists(filename))
    {
        BOOST_LOG_TRIVIAL(error) << __FUNCTION__ << ": file does not exist: " << filename;
        MessageDialog(this,
            _L("The selected file") + ":\n" + filename + "\n" + _L("does not exist."),
            wxString(GCODEVIEWER_APP_NAME) + " - " + _L("Error occurs while loading G-code file"),
            wxCLOSE | wxICON_WARNING | wxCENTRE).ShowModal();
        return;
    }

    m_last_loaded_gcode = filename;

    // BSS: create a new project when load_gcode, force close previous one
    if (new_project(false, true) != wxID_YES)
        return;

    m_only_gcode = true;

    // cleanup view before to start loading/processing
    //BBS: update gcode to current partplate's
    GCodeProcessorResult* current_result = p->partplate_list.get_current_slice_result();
    Print& current_print = p->partplate_list.get_current_fff_print();
    //BBS:already reset in new_project
    //current_result->reset();
    //p->gcode_result.reset();
    //reset_gcode_toolpaths();
    p->preview->reload_print(false, m_only_gcode);
    wxGetApp().mainframe->select_tab(MainFrame::tpPreview);
    p->set_current_panel(p->preview, true);
    GLCanvas3D* canvas = p->get_current_canvas3D();
    if (canvas)
        canvas->render();
    //p->notification_manager->bbl_show_plateinfo_notification(into_u8(_L("Preview only mode for gcode file.")));

    wxBusyCursor wait;

    // process gcode
    GCodeProcessor processor;
    try
    {
        GCodeProcessor::s_IsBBLPrinter = wxGetApp().preset_bundle->is_bbl_vendor();
        processor.process_file(filename.ToUTF8().data());
    }
    catch (const std::exception& ex)
    {
        show_error(this, ex.what());
        return;
    }
    *current_result = std::move(processor.extract_result());
    //current_result->filename = filename;

    BedType bed_type = current_result->bed_type;
    if (bed_type != BedType::btCount) {
        DynamicPrintConfig &proj_config = wxGetApp().preset_bundle->project_config;
        proj_config.set_key_value("curr_bed_type", new ConfigOptionEnum<BedType>(bed_type));
        on_bed_type_change(bed_type);
    }

    current_print.apply(this->model(), wxGetApp().preset_bundle->full_config());

    //BBS: add cost info when drag in gcode
    auto& ps = current_result->print_statistics;
    double total_cost = 0.0;
    for (auto volume : ps.total_volumes_per_extruder) {
        size_t extruder_id = volume.first;
        double density = current_result->filament_densities.at(extruder_id);
        double cost = current_result->filament_costs.at(extruder_id);
        double weight = volume.second * density * 0.001;
        total_cost += weight * cost * 0.001;
    }
    current_print.print_statistics().total_cost = total_cost;

    current_print.set_gcode_file_ready();

    // show results
    p->preview->reload_print(false, m_only_gcode);
    //BBS: zoom to bed 0 for gcode preview
    //p->preview->get_canvas3d()->zoom_to_gcode();
    p->preview->get_canvas3d()->zoom_to_plate(0);

    if (p->preview->get_canvas3d()->get_gcode_layers_zs().empty()) {
        MessageDialog(this, _L("The selected file") + ":\n" + filename + "\n" + _L("does not contain valid G-code."),
            wxString(GCODEVIEWER_APP_NAME) + " - " + _L("Error occurs while loading G-code file"), wxCLOSE | wxICON_WARNING | wxCENTRE).ShowModal();
        set_project_filename(DEFAULT_PROJECT_NAME);
    } else {
        set_project_filename(filename);
    }

    // Orca: Fix crash when loading gcode file multiple times
    if (m_only_gcode) {
        p->view3D->get_canvas3d()->remove_raycasters_for_picking(SceneRaycaster::EType::Bed);
    }
}

void Plater::reload_gcode_from_disk()
{
    wxString filename(m_last_loaded_gcode);
    m_last_loaded_gcode.clear();
    load_gcode(filename);
}

void Plater::refresh_print()
{
    p->preview->refresh_print();
}

// BBS
wxString Plater::get_project_name()
{
    return p->get_project_name();
}

void Plater::update_all_plate_thumbnails(bool force_update)
{
    for (int i = 0; i < get_partplate_list().get_plate_count(); i++) {
        PartPlate* plate = get_partplate_list().get_plate(i);
        ThumbnailsParams thumbnail_params = { {}, false, true, true, true, i};
        if (force_update || !plate->thumbnail_data.is_valid()) {
            get_view3D_canvas3D()->render_thumbnail(plate->thumbnail_data, plate->plate_thumbnail_width, plate->plate_thumbnail_height, thumbnail_params, Camera::EType::Ortho);
        }
        if (force_update || !plate->no_light_thumbnail_data.is_valid()) {
            get_view3D_canvas3D()->render_thumbnail(plate->no_light_thumbnail_data, plate->plate_thumbnail_width, plate->plate_thumbnail_height, thumbnail_params,
                                                    Camera::EType::Ortho,false,false,true);
        }
    }
}

//invalid all plate's thumbnails
void Plater::invalid_all_plate_thumbnails()
{
    if (using_exported_file() || skip_thumbnail_invalid)
        return;
    BOOST_LOG_TRIVIAL(info) << "thumb: invalid all";
    for (int i = 0; i < get_partplate_list().get_plate_count(); i++) {
        PartPlate* plate = get_partplate_list().get_plate(i);
        plate->thumbnail_data.reset();
        plate->no_light_thumbnail_data.reset();
    }
}

void Plater::force_update_all_plate_thumbnails()
{
    if (using_exported_file() || skip_thumbnail_invalid) {
    }
    else {
        invalid_all_plate_thumbnails();
        update_all_plate_thumbnails(true);
    }
    get_preview_canvas3D()->update_plate_thumbnails();
}

// BBS: backup
std::vector<size_t> Plater::load_files(const std::vector<fs::path>& input_files, LoadStrategy strategy, bool ask_multi) {
    //BBS: wish to reset state when load a new file
    p->m_slice_all_only_has_gcode = false;
    //BBS: wish to reset all plates stats item selected state when load a new file
    p->preview->get_canvas3d()->reset_select_plate_toolbar_selection();
    std::vector<size_t> loaded = p->load_files(input_files, strategy, ask_multi);
    if (!loaded.empty())
    {
        // After loading a project, initialize the filament temp mixing state
        // for ALL plates, not just the current one. This ensures each plate's
        // After loading a project, force a filament usage sync so that
        // the current plate's notification and slice button reflect the
        // filament temp mixing state. The blocking itself is enforced via
        // is_plate_blocked_by_filament_temp_mixing() independently of
        // m_apply_invalid; no per-plate flag initialization is needed.
        notify_filament_usage_changed();
        sync_filament_temp_mixing_notification();
        sync_flow_ratio_zero_notification();
        sync_cold_plate_notification();
    }
    return loaded;
}

// To be called when providing a list of files to the GUI slic3r on command line.
std::vector<size_t> Plater::load_files(const std::vector<std::string>& input_files, LoadStrategy strategy,  bool ask_multi)
{
    std::vector<fs::path> paths;
    paths.reserve(input_files.size());
    for (const std::string& path : input_files)
        paths.emplace_back(path);
    return load_files(paths, strategy, ask_multi);
}

bool Plater::preview_zip_archive(const boost::filesystem::path& archive_path)
{
    //std::vector<fs::path> unzipped_paths;
    std::vector<fs::path> non_project_paths;
    std::vector<fs::path> project_paths;
    try
    {
        mz_zip_archive archive;
        mz_zip_zero_struct(&archive);

        if (!open_zip_reader(&archive, archive_path.string())) {
            // TRN %1% is archive path
            std::string err_msg = GUI::format(_u8L("Loading of a ZIP archive on path %1% has failed."), archive_path.string());
            throw Slic3r::FileIOError(err_msg);
        }
        mz_uint num_entries = mz_zip_reader_get_num_files(&archive);
        mz_zip_archive_file_stat stat;
        // selected_paths contains paths and its uncompressed size. The size is used to distinguish between files with same path.
        std::vector<std::pair<fs::path, size_t>> selected_paths;
        FileArchiveDialog dlg(static_cast<wxWindow*>(wxGetApp().mainframe), &archive, selected_paths);
        if (dlg.ShowModal() == wxID_OK)
        {
            std::string archive_path_string = archive_path.string();
            archive_path_string = archive_path_string.substr(0, archive_path_string.size() - 4);
            fs::path archive_dir(wxStandardPaths::Get().GetTempDir().utf8_str().data());

            for (auto& path_w_size : selected_paths) {
                const fs::path& path = path_w_size.first;
                size_t size = path_w_size.second;
                // find path in zip archive
                for (mz_uint i = 0; i < num_entries; ++i) {
                    if (mz_zip_reader_file_stat(&archive, i, &stat)) {
                        if (size != stat.m_uncomp_size) // size must fit
                            continue;
                        wxString wname = boost::nowide::widen(stat.m_filename);
                        std::string name = into_u8(wname);
                        fs::path archive_path(name);

                        std::string extra(1024, 0);
                        size_t extra_size = mz_zip_reader_get_filename_from_extra(&archive, i, extra.data(), extra.size());
                        if (extra_size > 0) {
                            archive_path = fs::path(extra.substr(0, extra_size));
                            name = archive_path.string();
                        }

                        if (archive_path.empty())
                            continue;
                        if (path != archive_path)
                            continue;
                        // decompressing
                        try
                        {
                            std::replace(name.begin(), name.end(), '\\', '/');
                            // rename if file exists
                            std::string filename = path.filename().string();
                            std::string extension = path.extension().string();
                            std::string just_filename = filename.substr(0, filename.size() - extension.size());
                            std::string final_filename = just_filename;

                            size_t version = 0;
                            while (fs::exists(archive_dir / (final_filename + extension)))
                            {
                                ++version;
                                final_filename = just_filename + "(" + std::to_string(version) + ")";
                            }
                            filename = final_filename + extension;
                            fs::path final_path = archive_dir / filename;
                            std::string buffer((size_t)stat.m_uncomp_size, 0);
                            // Decompress action. We already has correct file index in stat structure.
                            mz_bool res = mz_zip_reader_extract_to_mem(&archive, stat.m_file_index, (void*)buffer.data(), (size_t)stat.m_uncomp_size, 0);
                            if (res == 0) {
                                // TRN: First argument = path to file, second argument = error description
                                wxString error_log = GUI::format_wxstr(_L("Failed to unzip file to %1%: %2%"), final_path.string(), mz_zip_get_error_string(mz_zip_get_last_error(&archive)));
                                BOOST_LOG_TRIVIAL(error) << error_log;
                                show_error(nullptr, error_log);
                                break;
                            }
                            // write buffer to file
                            fs::fstream file(final_path, std::ios::out | std::ios::binary | std::ios::trunc);
                            file.write(buffer.c_str(), buffer.size());
                            file.close();
                            if (!fs::exists(final_path)) {
                                wxString error_log = GUI::format_wxstr(_L("Failed to find unzipped file at %1%. Unzipping of file has failed."), final_path.string());
                                BOOST_LOG_TRIVIAL(error) << error_log;
                                show_error(nullptr, error_log);
                                break;
                            }
                            BOOST_LOG_TRIVIAL(info) << "Unzipped " << final_path;
                            if (!boost::algorithm::iends_with(filename, ".3mf") && !boost::algorithm::iends_with(filename, ".amf")) {
                                non_project_paths.emplace_back(final_path);
                                break;
                            }
                            // if 3mf - read archive headers to find project file
                            if (/*(boost::algorithm::iends_with(filename, ".3mf") && !is_project_3mf(final_path.string())) ||*/
                                (boost::algorithm::iends_with(filename, ".amf") && !boost::algorithm::iends_with(filename, ".zip.amf"))) {
                                non_project_paths.emplace_back(final_path);
                                break;
                            }

                            project_paths.emplace_back(final_path);
                            break;
                        }
                        catch (const std::exception& e)
                        {
                            // ensure the zip archive is closed and rethrow the exception
                            close_zip_reader(&archive);
                            throw Slic3r::FileIOError(e.what());
                        }
                    }
                }
            }
            close_zip_reader(&archive);
            if (non_project_paths.size() + project_paths.size() != selected_paths.size())
                BOOST_LOG_TRIVIAL(error) << "Decompresing of archive did not retrieve all files. Expected files: "
                                         << selected_paths.size()
                                         << " Decopressed files: "
                                         << non_project_paths.size() + project_paths.size();
        } else {
            close_zip_reader(&archive);
            return false;
        }

    }
    catch (const Slic3r::FileIOError& e) {
        // zip reader should be already closed or not even opened
        GUI::show_error(this, e.what());
        return false;
    }
    // none selected
    if (project_paths.empty() && non_project_paths.empty())
    {
        return false;
    }

    // 1 project file and some models - behave like drag n drop of 3mf and then load models
    if (project_paths.size() == 1)
    {
        wxArrayString aux;
        aux.Add(from_u8(project_paths.front().string()));
        bool loaded3mf = load_files(aux);
        load_files(non_project_paths, LoadStrategy::LoadModel);
        boost::system::error_code ec;
        if (loaded3mf) {
            fs::remove(project_paths.front(), ec);
            if (ec)
                BOOST_LOG_TRIVIAL(error) << ec.message();
        }
        for (const fs::path& path : non_project_paths) {
            // Delete file from temp file (path variable), it will stay only in app memory.
            boost::system::error_code ec;
            fs::remove(path, ec);
            if (ec)
                BOOST_LOG_TRIVIAL(error) << ec.message();
        }
        return true;
    }

    // load all projects and all models as geometry
    load_files(project_paths, LoadStrategy::LoadModel);
    load_files(non_project_paths, LoadStrategy::LoadModel);


    for (const fs::path& path : project_paths) {
        // Delete file from temp file (path variable), it will stay only in app memory.
        boost::system::error_code ec;
        fs::remove(path, ec);
        if (ec)
            BOOST_LOG_TRIVIAL(error) << ec.message();
    }
    for (const fs::path& path : non_project_paths) {
        // Delete file from temp file (path variable), it will stay only in app memory.
        boost::system::error_code ec;
        fs::remove(path, ec);
        if (ec)
            BOOST_LOG_TRIVIAL(error) << ec.message();
    }

    return true;
}

#define PROJECT_DROP_DIALOG_SELECT_PLANE_SIZE wxSize(FromDIP(350), FromDIP(120))

class ProjectDropDialog : public DPIDialog
{
private:
    wxColour          m_def_color = wxColour(255, 255, 255);
    int               m_action{1};
    bool              m_remember_choice{false};

public:
    ProjectDropDialog(const std::string &filename);

    wxPanel *     m_top_line;
    wxStaticText *m_fname_title;
    wxStaticText *m_fname_f;
    wxStaticText *m_fname_s;
    StaticBox * m_panel_select;

    void      on_select_ok(wxCommandEvent &event);
    void      on_select_cancel(wxCommandEvent &event);

    int       get_action() const { return m_action; }
    void      set_action(int index) { m_action = index; }

    wxBoxSizer *create_remember_checkbox(wxString title, wxWindow* parent, wxString tooltip);
    wxBoxSizer *create_item_radiobox(wxString title, wxWindow *parent, int select_id, int groupid);

protected:
    void on_dpi_changed(const wxRect &suggested_rect) override;
};

ProjectDropDialog::ProjectDropDialog(const std::string &filename)
    : DPIDialog(static_cast<wxWindow *>(wxGetApp().mainframe),
                wxID_ANY,
                from_u8((boost::format(_utf8(L("Drop project file")))).str()),
                wxDefaultPosition,
                wxDefaultSize,
                wxCAPTION | wxCLOSE_BOX)
    , m_action(2)
{
    // def setting
    SetBackgroundColour(m_def_color);

    // icon
    std::string icon_path = (boost::format("%1%/images/Snapmaker_OrcaTitle.ico") % resources_dir()).str();
    SetIcon(wxIcon(encode_path(icon_path.c_str()), wxBITMAP_TYPE_ICO));

    wxBoxSizer *m_sizer_main = new wxBoxSizer(wxVERTICAL);

    m_top_line = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    m_top_line->SetBackgroundColour(wxColour(166, 169, 170));

    m_sizer_main->Add(m_top_line, 0, wxEXPAND, 0);

    m_sizer_main->Add(0, 0, 0, wxEXPAND | wxTOP, 20);

    wxBoxSizer *m_sizer_name = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *m_sizer_fline = new wxBoxSizer(wxHORIZONTAL);

    m_fname_title = new wxStaticText(this, wxID_ANY, _L("Please select an action"), wxDefaultPosition, wxDefaultSize, 0);
    m_fname_title->Wrap(-1);
    m_fname_title->SetFont(::Label::Body_14);
    m_fname_title->SetForegroundColour(wxColour(107, 107, 107));
    m_fname_title->SetBackgroundColour(wxColour(255, 255, 255));

    m_sizer_fline->Add(m_fname_title, 0, wxALL, 0);
    m_sizer_fline->Add(0, 0, 0, wxEXPAND | wxLEFT, 5);

    m_fname_f = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    m_fname_f->SetFont(::Label::Head_14);
    m_fname_f->Wrap(-1);
    m_fname_f->SetForegroundColour(wxColour(38, 46, 48));

    m_sizer_fline->Add(m_fname_f, 1, wxALL, 0);

    m_sizer_name->Add(m_sizer_fline, 1, wxEXPAND, 0);

    m_fname_s = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    m_fname_s->SetFont(::Label::Head_14);
    m_fname_s->Wrap(-1);
    m_fname_s->SetForegroundColour(wxColour(38, 46, 48));

    m_sizer_name->Add(m_fname_s, 1, wxALL, 0);

    m_sizer_main->Add(m_sizer_name, 1, wxEXPAND | wxLEFT | wxRIGHT, 20);

    auto radio_group = new RadioGroup(this, {
        _L("Open as project"),     // 0
        _L("Import geometry only") // 1
    }, wxVERTICAL);
    radio_group->SetMinSize(wxSize(FromDIP(300),-1));
    radio_group->SetSelection(get_action() - 1);
    radio_group->Bind(wxEVT_COMMAND_RADIOBOX_SELECTED, [this, radio_group](wxCommandEvent &e) {
        set_action(radio_group->GetSelection() + 1);
    });

    m_sizer_main->Add(radio_group, 0, wxEXPAND | wxLEFT | wxRIGHT, 20);

    m_sizer_main->Add(0, 0, 0, wxEXPAND | wxTOP, 10);

    // wxBoxSizer *m_sizer_bottom = new wxBoxSizer(wxHORIZONTAL);
    // Orca: hide the "Don't show again" checkbox, people keeps accidentally checked this then forgot
    // wxBoxSizer *m_sizer_left = new wxBoxSizer(wxHORIZONTAL);
    //
    // auto dont_show_again = create_remember_checkbox(_L("Remember my choice."), this, _L("This option can be changed later in preferences, under 'Load Behaviour'."));
    // m_sizer_left->Add(dont_show_again, 0, wxALL, 5);
    //
    // m_sizer_bottom->Add(m_sizer_left, 0, wxEXPAND, 5);

    auto dlg_btns = new DialogButtons(this, {"OK", "Cancel"});

    dlg_btns->GetOK()->Bind(wxEVT_BUTTON, &ProjectDropDialog::on_select_ok, this);

    dlg_btns->GetCANCEL()->Bind(wxEVT_BUTTON, &ProjectDropDialog::on_select_cancel, this);

    m_sizer_main->Add(dlg_btns, 0, wxEXPAND);

    SetSizer(m_sizer_main);
    Layout();
    Fit();
    Centre(wxBOTH);


    auto limit_width   = m_fname_f->GetSize().GetWidth() - 2;
    auto current_width = 0;
    auto cut_index     = 0;
    auto fstring       = wxString("");
    auto bstring       = wxString("");

    //auto file_name = from_u8(filename.c_str());
    auto file_name = wxString(filename);
    for (int x = 0; x < file_name.length(); x++) {
        current_width += m_fname_s->GetTextExtent(file_name[x]).GetWidth();
        cut_index = x;

        if (current_width > limit_width) {
            bstring += file_name[x];
        } else {
            fstring += file_name[x];
        }
    }

    m_fname_f->SetLabel(fstring);
    m_fname_s->SetLabel(bstring);

    wxGetApp().UpdateDlgDarkUI(this);
}

wxBoxSizer *ProjectDropDialog::create_remember_checkbox(wxString title, wxWindow *parent, wxString tooltip)
{
    wxBoxSizer *m_sizer_checkbox = new wxBoxSizer(wxHORIZONTAL);
    m_sizer_checkbox->Add(0, 0, 0, wxEXPAND | wxLEFT, 5);

    auto checkbox = new ::CheckBox(parent);
    checkbox->SetValue(m_remember_choice);
    checkbox->SetToolTip(tooltip);
    m_sizer_checkbox->Add(checkbox, 0, wxALIGN_CENTER, 0);
    m_sizer_checkbox->Add(0, 0, 0, wxEXPAND | wxLEFT, 8);

    auto checkbox_title = new wxStaticText(parent, wxID_ANY, title, wxDefaultPosition, wxSize(-1, -1), 0);
    checkbox_title->SetForegroundColour(wxColour(144,144,144));
    checkbox_title->SetFont(::Label::Body_13);
    checkbox_title->Wrap(-1);
    checkbox_title->SetToolTip(tooltip);
    m_sizer_checkbox->Add(checkbox_title, 0, wxALIGN_CENTER | wxALL, 3);

    checkbox->Bind(wxEVT_TOGGLEBUTTON, [this, checkbox](wxCommandEvent &e) {
        m_remember_choice = checkbox->GetValue();
        e.Skip();
    });

    return m_sizer_checkbox;
}

void ProjectDropDialog::on_select_ok(wxCommandEvent &event)
{
    if (m_remember_choice) {
        LoadType load_type = static_cast<LoadType>(get_action());
        switch (load_type)
        {
            case LoadType::OpenProject:
                wxGetApp().app_config->set(SETTING_PROJECT_LOAD_BEHAVIOUR, OPTION_PROJECT_LOAD_BEHAVIOUR_LOAD_ALL);
                break;
            case LoadType::LoadGeometry:
                wxGetApp().app_config->set(SETTING_PROJECT_LOAD_BEHAVIOUR, OPTION_PROJECT_LOAD_BEHAVIOUR_LOAD_GEOMETRY);
                break;
        }
    }

    EndModal(wxID_OK);
}

void ProjectDropDialog::on_select_cancel(wxCommandEvent &event)
{
    EndModal(wxID_CANCEL);
}

void ProjectDropDialog::on_dpi_changed(const wxRect& suggested_rect)
{
    Fit();
    Refresh();
}

//BBS: remove GCodeViewer as seperate APP logic
bool Plater::load_files(const wxArrayString& filenames)
{
    const std::regex pattern_drop(".*[.](stp|step|stl|oltp|obj|amf|3mf|svg|zip)", std::regex::icase);
    const std::regex pattern_gcode_drop(".*[.](gcode|g)", std::regex::icase);

    std::vector<fs::path> normal_paths;
    std::vector<fs::path> gcode_paths;

    for (const auto& filename : filenames) {
        fs::path path(into_path(filename));
        if (std::regex_match(path.string(), pattern_drop))
            normal_paths.push_back(std::move(path));
        else if (std::regex_match(path.string(), pattern_gcode_drop))
            gcode_paths.push_back(std::move(path));
        else
            continue;
    }

    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": normal_paths %1%, gcode_paths %2%")%normal_paths.size() %gcode_paths.size();
    if (normal_paths.empty() && gcode_paths.empty()) {
        BOOST_LOG_TRIVIAL(error) << __FUNCTION__ << boost::format(": can not find valid path, return directly");
        // Likely no supported files
        return false;
    }
    else if (normal_paths.empty()){
        //only gcode files
        if (gcode_paths.size() > 1) {
            show_info(this, _L("Only one G-code file can be opened at the same time."), _L("G-code loading"));
            return false;
        }
        load_gcode(from_path(gcode_paths.front()));
        return true;
    }

    if (!gcode_paths.empty()) {
        show_info(this, _L("G-code files cannot be loaded with models together!"), _L("G-code loading"));
        return false;
    }

    //// searches for project files
    //for (std::vector<fs::path>::const_reverse_iterator it = normal_paths.rbegin(); it != normal_paths.rend(); ++it) {
    //    std::string filename = (*it).filename().string();
    //    ////BBS: only 3mf will be treated as project file
    //    if (open_3mf_file((*it)))
    //        return true;
    //}

    //// other files
    std::string snapshot_label;
    assert(!normal_paths.empty());
    if (normal_paths.size() == 1) {
        snapshot_label = "Load File";
        snapshot_label += ": ";
        snapshot_label += encode_path(normal_paths.front().filename().string().c_str());
    } else {
        snapshot_label = "Load Files";
        snapshot_label += ": ";
        snapshot_label += encode_path(normal_paths.front().filename().string().c_str());
        for (size_t i = 1; i < normal_paths.size(); ++i) {
            snapshot_label += ", ";
            snapshot_label += encode_path(normal_paths[i].filename().string().c_str());
        }
    }

    //Plater::TakeSnapshot snapshot(this, snapshot_label);
    //load_files(normal_paths, LoadStrategy::LoadModel);

    // BBS: check file types
    std::sort(normal_paths.begin(), normal_paths.end(), [](fs::path obj1, fs::path obj2) { return obj1.filename().string() < obj2.filename().string(); });

    auto loadfiles_type  = LoadFilesType::NoFile;
    auto amf_files_count = get_3mf_file_count(normal_paths);

    if (normal_paths.size() > 1 && amf_files_count < normal_paths.size()) { loadfiles_type = LoadFilesType::Multiple3MFOther; }
    if (normal_paths.size() > 1 && amf_files_count == normal_paths.size()) { loadfiles_type = LoadFilesType::Multiple3MF; }
    if (normal_paths.size() > 1 && amf_files_count == 0) { loadfiles_type = LoadFilesType::MultipleOther; }
    if (normal_paths.size() == 1 && amf_files_count == 1) { loadfiles_type = LoadFilesType::Single3MF; };
    if (normal_paths.size() == 1 && amf_files_count == 0) { loadfiles_type = LoadFilesType::SingleOther; };

    auto first_file = std::vector<fs::path>{};
    auto tmf_file   = std::vector<fs::path>{};
    auto other_file = std::vector<fs::path>{};
    auto res        = true;

    if (this->m_only_gcode || this->m_exported_file) {
        if ((loadfiles_type == LoadFilesType::SingleOther)
            || (loadfiles_type == LoadFilesType::MultipleOther)) {
            show_info(this, _L("Cannot add models when in preview mode!"), _L("Add Models"));
            return false;
        }
    }

    // Orca: Iters through given paths and imports files from zip then remove zip from paths
    // returns true if zip files were found
    auto handle_zips = [this](vector<fs::path>& paths) { // NOLINT(*-no-recursion) - Recursion is intended and should be managed properly
        bool res = false;
        for (auto it = paths.begin(); it != paths.end();) {
            if (boost::algorithm::iends_with(it->string(), ".zip")) {
                res = true;
                preview_zip_archive(*it);
                it = paths.erase(it);
            } else
                it++;
        }
        return res;
    };

    switch (loadfiles_type) {
    case LoadFilesType::Single3MF:
        open_3mf_file(normal_paths[0]);
        break;

    case LoadFilesType::SingleOther: {
        Plater::TakeSnapshot snapshot(this, snapshot_label);
        if (handle_zips(normal_paths)) return true;
        if (load_files(normal_paths, LoadStrategy::LoadModel, false).empty()) { res = false; }
        break;
    }
    case LoadFilesType::Multiple3MF:
        first_file = std::vector<fs::path>{normal_paths[0]};
        for (auto i = 0; i < normal_paths.size(); i++) {
            if (i > 0) { other_file.push_back(normal_paths[i]); }
        };

        open_3mf_file(first_file[0]);
        if (load_files(other_file, LoadStrategy::LoadModel).empty()) {  res = false;  }
        break;

    case LoadFilesType::MultipleOther: {
        Plater::TakeSnapshot snapshot(this, snapshot_label);
        if (handle_zips(normal_paths)) {
            if (normal_paths.empty()) return true;
        }
        if (load_files(normal_paths, LoadStrategy::LoadModel, true).empty()) { res = false; }
        break;
    }

    case LoadFilesType::Multiple3MFOther:
        for (const auto &path : normal_paths) {
            if (boost::iends_with(path.filename().string(), ".3mf")){
                if (first_file.size() <= 0)
                    first_file.push_back(path);
                else
                    tmf_file.push_back(path);
            } else {
                other_file.push_back(path);
            }
        }

        open_3mf_file(first_file[0]);
        if (load_files(tmf_file, LoadStrategy::LoadModel).empty()) {  res = false;  }
        if (res && handle_zips(other_file)) {
            if (normal_paths.empty()) return true;
        }
        if (load_files(other_file, LoadStrategy::LoadModel, false).empty()) {  res = false;  }
        break;
    default: break;
    }

    return res;
}

LoadType determine_load_type(std::string filename, std::string override_setting)
{
    std::string setting;

    if (override_setting != "") {
        setting = override_setting;
    } else {
        setting = wxGetApp().app_config->get(SETTING_PROJECT_LOAD_BEHAVIOUR);
    }

    if (setting == OPTION_PROJECT_LOAD_BEHAVIOUR_LOAD_GEOMETRY) {
        return LoadType::LoadGeometry;
    } else if (setting == OPTION_PROJECT_LOAD_BEHAVIOUR_ALWAYS_ASK) {
        ProjectDropDialog dlg(filename);
        if (dlg.ShowModal() == wxID_OK) {
            int      choice    = dlg.get_action();
            LoadType load_type = static_cast<LoadType>(choice);
            wxGetApp().app_config->set("import_project_action", std::to_string(choice));

            // BBS: jump to plater panel
            wxGetApp().mainframe->select_tab(MainFrame::tp3DEditor);
            return load_type;
        }

        return LoadType::Unknown; // Cancel
    } else {
        return LoadType::OpenProject;
    }
}

bool Plater::open_3mf_file(const fs::path &file_path)
{
    std::string filename = encode_path(file_path.filename().string().c_str());
    if (!boost::algorithm::iends_with(filename, ".3mf")) {
        return false;
    }

    bool not_empty_plate = !model().objects.empty();
    bool load_setting_ask_when_relevant = wxGetApp().app_config->get(SETTING_PROJECT_LOAD_BEHAVIOUR) == OPTION_PROJECT_LOAD_BEHAVIOUR_ASK_WHEN_RELEVANT;
    LoadType load_type = determine_load_type(filename, (not_empty_plate && load_setting_ask_when_relevant) ? OPTION_PROJECT_LOAD_BEHAVIOUR_ALWAYS_ASK : "");

    if (load_type == LoadType::Unknown) return false;

    switch (load_type) {
        case LoadType::OpenProject: {
            if (wxGetApp().can_load_project())
                load_project(from_path(file_path), "<loadall>");
            break;
        }
        case LoadType::LoadGeometry: {
            Plater::TakeSnapshot snapshot(this, "Import Object");
            load_files({file_path}, LoadStrategy::LoadModel);
            break;
        }
        case LoadType::LoadConfig: {
            load_files({file_path}, LoadStrategy::LoadConfig);
            break;
        }
        case LoadType::Unknown: {
            assert(false);
            break;
        }
    }

    return true;
}

int Plater::get_3mf_file_count(std::vector<fs::path> paths)
{
    auto count = 0;
    for (const auto &path : paths) {
        if (boost::iends_with(path.filename().string(), ".3mf")) {
            count++;
        }
    }
    return count;
}

void Plater::add_file()
{
    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << __LINE__ << " entry";
    wxArrayString input_files;
    wxGetApp().import_model(this, input_files);
    if (input_files.empty()) return;

    std::vector<fs::path> paths;
    for (const auto &file : input_files) paths.emplace_back(into_path(file));

    std::string snapshot_label;
    assert(!paths.empty());

    snapshot_label = "Import Objects";
    snapshot_label += ": ";
    snapshot_label += encode_path(paths.front().filename().string().c_str());
    for (size_t i = 1; i < paths.size(); ++i) {
        snapshot_label += ", ";
        snapshot_label += encode_path(paths[i].filename().string().c_str());
    }

    // BBS: check file types
    auto loadfiles_type  = LoadFilesType::NoFile;
    auto amf_files_count = get_3mf_file_count(paths);

    if (paths.size() > 1 && amf_files_count < paths.size()) { loadfiles_type = LoadFilesType::Multiple3MFOther; }
    if (paths.size() > 1 && amf_files_count == paths.size()) { loadfiles_type = LoadFilesType::Multiple3MF; }
    if (paths.size() > 1 && amf_files_count == 0) { loadfiles_type = LoadFilesType::MultipleOther; }
    if (paths.size() == 1 && amf_files_count == 1) { loadfiles_type = LoadFilesType::Single3MF; };
    if (paths.size() == 1 && amf_files_count == 0) { loadfiles_type = LoadFilesType::SingleOther; };

    auto first_file = std::vector<fs::path>{};
    auto tmf_file   = std::vector<fs::path>{};
    auto other_file = std::vector<fs::path>{};

    switch (loadfiles_type)
    {
    case LoadFilesType::Single3MF:
        open_3mf_file(paths[0]);
    	break;

    case LoadFilesType::SingleOther: {
        Plater::TakeSnapshot snapshot(this, snapshot_label);
        if (!load_files(paths, LoadStrategy::LoadModel, false).empty()) {
            if (get_project_name() == _L("Untitled") && paths.size() > 0) {
                boost::filesystem::path full_path(paths[0].string());
                p->set_project_name(from_u8(full_path.stem().string()));
            }
            wxGetApp().mainframe->update_title();
            if (wxGetApp().app_config->get("recent_models") == "true")
                wxGetApp().mainframe->add_to_recent_projects(paths[0].wstring());
        }
        break;
    }
    case LoadFilesType::Multiple3MF:
        first_file = std::vector<fs::path>{paths[0]};
        for (auto i = 0; i < paths.size(); i++) {
            if (i > 0) { other_file.push_back(paths[i]); }
        };

        open_3mf_file(first_file[0]);
        if (!load_files(other_file, LoadStrategy::LoadModel).empty()) { wxGetApp().mainframe->update_title(); }
        break;

    case LoadFilesType::MultipleOther: {
        Plater::TakeSnapshot snapshot(this, snapshot_label);
        if (!load_files(paths, LoadStrategy::LoadModel, true).empty()) {
            if (get_project_name() == _L("Untitled") && paths.size() > 0) {
                boost::filesystem::path full_path(paths[0].string());
                p->set_project_name(from_u8(full_path.stem().string()));
            }
            wxGetApp().mainframe->update_title();
            if (wxGetApp().app_config->get("recent_models") == "true")
                for (auto &path : paths)
                    wxGetApp().mainframe->add_to_recent_projects(path.wstring());
        }
        break;
    }
    case LoadFilesType::Multiple3MFOther:
        for (const auto &path : paths) {
            if (boost::iends_with(path.filename().string(), ".3mf")) {
                if (first_file.size() <= 0)
                    first_file.push_back(path);
                else
                    tmf_file.push_back(path);
            } else {
                other_file.push_back(path);
            }
        }

        open_3mf_file(first_file[0]);
        load_files(tmf_file, LoadStrategy::LoadModel);
        if (!load_files(other_file, LoadStrategy::LoadModel, false).empty()) {
            wxGetApp().mainframe->update_title();
            if (wxGetApp().app_config->get("recent_models") == "true")
                for (auto &file : other_file)
                    wxGetApp().mainframe->add_to_recent_projects(file.wstring());
        }
        break;
    default:break;
    }
}

void Plater::update(bool conside_update_flag, bool force_background_processing_update)
{
    unsigned int flag = force_background_processing_update ? (unsigned int)Plater::priv::UpdateParams::FORCE_BACKGROUND_PROCESSING_UPDATE : 0;
    if (conside_update_flag) {
        if (need_update()) {
            p->update(flag);
            p->set_need_update(false);
        }
    }
    else
        p->update(flag);
}

void Plater::object_list_changed() { p->object_list_changed(); }

Worker &Plater::get_ui_job_worker() { return p->m_worker; }

const Worker &Plater::get_ui_job_worker() const { return p->m_worker; }

void Plater::update_ui_from_settings() { p->update_ui_from_settings(); }

void Plater::select_view(const std::string& direction) { p->select_view(direction); }

//BBS: add no_slice logic
void Plater::select_view_3D(const std::string& name, bool no_slice) { p->select_view_3D(name, no_slice); }

void Plater::reload_paint_after_background_process_apply() {
    p->preview->set_reload_paint_after_background_process_apply(true);
}

bool Plater::is_preview_shown() const { return p->is_preview_shown(); }
bool Plater::is_preview_loaded() const { return p->is_preview_loaded(); }
bool Plater::is_view3D_shown() const { return p->is_view3D_shown(); }

bool Plater::are_view3D_labels_shown() const { return p->are_view3D_labels_shown(); }
void Plater::show_view3D_labels(bool show) { p->show_view3D_labels(show); }

bool Plater::is_view3D_overhang_shown() const { return p->is_view3D_overhang_shown(); }
void Plater::show_view3D_overhang(bool show)  {  p->show_view3D_overhang(show); }

bool Plater::is_sidebar_enabled() const { return p->sidebar_layout.is_enabled; }
void Plater::enable_sidebar(bool enabled) { p->enable_sidebar(enabled); }
bool Plater::is_sidebar_collapsed() const { return p->sidebar_layout.is_collapsed; }
void Plater::collapse_sidebar(bool collapse) { p->collapse_sidebar(collapse); }
Sidebar::DockingState Plater::get_sidebar_docking_state() const { return p->get_sidebar_docking_state(); }

void Plater::reset_window_layout() { p->reset_window_layout(); }

//BBS
void Plater::select_curr_plate_all() { p->select_curr_plate_all(); }
void Plater::remove_curr_plate_all() { p->remove_curr_plate_all(); }

void Plater::select_all() { p->select_all(); }
void Plater::deselect_all() { p->deselect_all(); }
void Plater::exit_gizmo() { p->exit_gizmo(); }

void Plater::remove(size_t obj_idx) { p->remove(obj_idx); }
void Plater::reset(bool apply_presets_change) { p->reset(apply_presets_change); }
void Plater::reset_with_confirm()
{
    if (p->model.objects.empty() || MessageDialog(static_cast<wxWindow *>(this), _L("All objects will be removed, continue?"),
                                                  wxString(SLIC3R_APP_FULL_NAME) + " - " + _L("Delete all"), wxYES_NO | wxCANCEL | wxYES_DEFAULT | wxCENTRE)
                                            .ShowModal() == wxID_YES) {
        reset();
        // BBS: jump to plater panel
        wxGetApp().mainframe->select_tab(size_t(0));
    }
}

// BBS: save logic
int GUI::Plater::close_with_confirm(std::function<bool(bool)> second_check)
{
    if (up_to_date(false, false)) {
        if (second_check && !second_check(false)) return wxID_CANCEL;
        model().set_backup_path("");
        return wxID_NO;
    }

    MessageDialog dlg(static_cast<wxWindow*>(this), _L("The current project has unsaved changes, save it before continue?"),
        wxString(SLIC3R_APP_FULL_NAME) + " - " + _L("Save"), wxYES_NO | wxCANCEL | wxYES_DEFAULT | wxCENTRE);
    dlg.show_dsa_button(_L("Remember my choice."));
    auto choise = wxGetApp().app_config->get("save_project_choise");
    auto result = choise.empty() ? dlg.ShowModal() : choise == "yes" ? wxID_YES : wxID_NO;
    if (result == wxID_CANCEL)
        return result;
    else {
        if (dlg.get_checkbox_state())
            wxGetApp().app_config->set("save_project_choise", result == wxID_YES ? "yes" : "no");
        if (result == wxID_YES) {
            result = save_project();
            if (result == wxID_CANCEL) {
                if (choise.empty())
                    return result;
                else
                    result = wxID_NO;
            }
        }
    }

    if (second_check && !second_check(result == wxID_YES)) return wxID_CANCEL;

    model().set_backup_path("");
    up_to_date(true, false);
    up_to_date(true, true);

    return result;
}

//BBS: trigger a restore project event
void Plater::trigger_restore_project(int skip_confirm)
{
    auto evt = new wxCommandEvent(EVT_RESTORE_PROJECT, this->GetId());
    evt->SetInt(skip_confirm);
    wxQueueEvent(this, evt);
    //wxPostEvent(this, *evt);
}

//BBS
bool Plater::delete_object_from_model(size_t obj_idx, bool refresh_immediately) { return p->delete_object_from_model(obj_idx, refresh_immediately); }

//BBS: delete all from model
void Plater::delete_all_objects_from_model()
{
    p->delete_all_objects_from_model();
}

void Plater::set_selected_visible(bool visible)
{
    if (p->get_curr_selection().is_empty())
        return;

    Plater::TakeSnapshot snapshot(this, "Set Selected Objects Visible in AssembleView");
    get_ui_job_worker().cancel_all();

    GLCanvas3D* canvas = p->get_current_canvas3D();
    if (canvas)
        canvas->set_selected_visible(visible);
}


void Plater::remove_selected()
{
    /*if (p->get_selection().is_empty())
        return;*/
    if (p->get_curr_selection().is_empty())
        return;

    // BBS: check before deleting object
    if (!p->can_delete())
        return;

    Plater::TakeSnapshot snapshot(this, "Delete Selected Objects");
    get_ui_job_worker().cancel_all();

    //BBS delete current selected
    // p->view3D->delete_selected();
    GLCanvas3D* canvas = p->get_current_canvas3D();
    if (canvas) {
        canvas->delete_selected();
        notify_filament_usage_changed();
    }
}

void Plater::increase_instances(size_t num)
{
    // BBS
#if 0
    if (! can_increase_instances()) { return; }

    Plater::TakeSnapshot snapshot(this, "Increase Instances");

    int obj_idx = p->get_selected_object_idx();

    ModelObject* model_object = p->model.objects[obj_idx];
    ModelInstance* model_instance = model_object->instances.back();

    bool was_one_instance = model_object->instances.size()==1;

    double offset_base = canvas3D()->get_size_proportional_to_max_bed_size(0.05);
    double offset = offset_base;
    for (size_t i = 0; i < num; i++, offset += offset_base) {
        Vec3d offset_vec = model_instance->get_offset() + Vec3d(offset, offset, 0.0);
        model_object->add_instance(offset_vec, model_instance->get_scaling_factor(), model_instance->get_rotation(), model_instance->get_mirror());
//        p->print.get_object(obj_idx)->add_copy(Slic3r::to_2d(offset_vec));
    }

#ifdef SUPPORT_AUTO_CENTER
    if (p->get_config("autocenter") == "true")
        arrange();
#endif

    p->update();

    p->get_selection().add_instance(obj_idx, (int)model_object->instances.size() - 1);

    sidebar().obj_list()->increase_object_instances(obj_idx, was_one_instance ? num + 1 : num);

    p->selection_changed();
    this->p->schedule_background_process();
#endif
}

void Plater::decrease_instances(size_t num)
{
    // BBS
#if 0
    if (! can_decrease_instances()) { return; }

    Plater::TakeSnapshot snapshot(this, "Decrease Instances");

    int obj_idx = p->get_selected_object_idx();

    ModelObject* model_object = p->model.objects[obj_idx];
    if (model_object->instances.size() > num) {
        for (size_t i = 0; i < num; ++ i)
            model_object->delete_last_instance();
        p->update();
        // Delete object from Sidebar list. Do it after update, so that the GLScene selection is updated with the modified model.
        sidebar().obj_list()->decrease_object_instances(obj_idx, num);
    }
    else {
        remove(obj_idx);
    }

    if (!model_object->instances.empty())
        p->get_selection().add_instance(obj_idx, (int)model_object->instances.size() - 1);

    p->selection_changed();
    this->p->schedule_background_process();
#endif
}

static long GetNumberFromUser(  const wxString& msg,
                                const wxString& prompt,
                                const wxString& title,
                                long value,
                                long min,
                                long max,
                                wxWindow* parent)
{
#ifdef _WIN32
    wxNumberEntryDialog dialog(parent, msg, prompt, title, value, min, max, wxDefaultPosition);
    wxGetApp().UpdateDlgDarkUI(&dialog);
    if (dialog.ShowModal() == wxID_OK)
        return dialog.GetValue();

    return -1;
#else
    return wxGetNumberFromUser(msg, prompt, title, value, min, max, parent);
#endif
}

void Plater::set_number_of_copies(/*size_t num*/)
{
    int obj_idx = p->get_selected_object_idx();
    if (obj_idx == -1)
        return;

    ModelObject* model_object = p->model.objects[obj_idx];

    const int num = GetNumberFromUser( " ", _L("Number of copies:"),
                                    _L("Copies of the selected object"), model_object->instances.size(), 0, 1000, this );
    if (num < 0)
        return;

    Plater::TakeSnapshot snapshot(this, (boost::format("Set numbers of copies to %1%")%num).str());

    int diff = num - (int)model_object->instances.size();
    if (diff > 0)
        increase_instances(diff);
    else if (diff < 0)
        decrease_instances(-diff);
}

void Plater::fill_bed_with_instances()
{
    auto &w = get_ui_job_worker();
    if (w.is_idle()) {
        p->take_snapshot(_u8L("Arrange"));
        replace_job(w, std::make_unique<FillBedJob>());
    }
}

bool Plater::is_selection_empty() const
{
    return p->get_selection().is_empty() || p->get_selection().is_wipe_tower();
}

void Plater::scale_selection_to_fit_print_volume()
{
    p->scale_selection_to_fit_print_volume();
}

void Plater::convert_unit(ConversionType conv_type)
{
    std::vector<int> obj_idxs, volume_idxs;
    wxGetApp().obj_list()->get_selection_indexes(obj_idxs, volume_idxs);
    if (obj_idxs.empty() && volume_idxs.empty())
        return;

    TakeSnapshot snapshot(this, conv_type == ConversionType::CONV_FROM_INCH  ? "Convert from imperial units" :
                                conv_type == ConversionType::CONV_TO_INCH    ? "Revert conversion from imperial units" :
                                conv_type == ConversionType::CONV_FROM_METER ? "Convert from meters" : "Revert conversion from meters");
    wxBusyCursor wait;

    ModelObjectPtrs objects;
    std::reverse(obj_idxs.begin(), obj_idxs.end());
    for (int obj_idx : obj_idxs) {
        ModelObject *object = p->model.objects[obj_idx];
        object->convert_units(objects, conv_type, volume_idxs);
        remove(obj_idx);
    }
    std::reverse(objects.begin(), objects.end());
    p->load_model_objects(objects);

    Selection& selection = p->view3D->get_canvas3d()->get_selection();
    size_t last_obj_idx = p->model.objects.size() - 1;

    if (volume_idxs.empty()) {
        for (size_t i = 0; i < objects.size(); ++i)
            selection.add_object((unsigned int)(last_obj_idx - i), i == 0);
    }
    else {
        for (int vol_idx : volume_idxs)
            selection.add_volume(last_obj_idx, vol_idx, 0, false);
    }
}

void Plater::apply_cut_object_to_model(size_t obj_idx, const ModelObjectPtrs& new_objects)
{
    model().delete_object(obj_idx);
    sidebar().obj_list()->delete_object_from_list(obj_idx);

    // suppress to call selection update for Object List to avoid call of early Gizmos on/off update
    p->load_model_objects(new_objects, false, false);

    // now process all updates of the 3d scene
    update();
    // Update InfoItems in ObjectList after update() to use of a correct value of the GLCanvas3D::is_sinking(),
    // which is updated after a view3D->reload_scene(false, flags & (unsigned int)UpdateParams::FORCE_FULL_SCREEN_REFRESH) call
    for (size_t idx = 0; idx < p->model.objects.size(); idx++)
        wxGetApp().obj_list()->update_info_items(idx);

    Selection& selection = p->get_selection();
    size_t last_id = p->model.objects.size() - 1;
    for (size_t i = 0; i < new_objects.size(); ++i)
        selection.add_object((unsigned int)(last_id - i), i == 0);

    // UIThreadWorker w;
    // arrange(w, true);
    // w.wait_for_idle();
}

void Plater::export_gcode(bool prefer_removable)
{
    if (p->model.objects.empty())
        return;

    //if (get_view3D_canvas3D()->get_gizmos_manager().is_in_editing_mode(true))
    //    return;

    if (p->process_completed_with_error == p->partplate_list.get_curr_plate_index())
        return;

    // If possible, remove accents from accented latin characters.
    // This function is useful for generating file names to be processed by legacy firmwares.
    fs::path default_output_file;
    try {
        // Update the background processing, so that the placeholder parser will get the correct values for the ouput file template.
        // Also if there is something wrong with the current configuration, a pop-up dialog will be shown and the export will not be performed.
        unsigned int state = this->p->update_restart_background_process(false, false);
        if (state & priv::UPDATE_BACKGROUND_PROCESS_INVALID)
            return;
        default_output_file = this->p->background_process.output_filepath_for_project("");
    } catch (const Slic3r::PlaceholderParserError &ex) {
        // Show the error with monospaced font.
        show_error(this, ex.what(), true);
        return;
    } catch (const std::exception &ex) {
        show_error(this, ex.what(), false);
        return;
    }
    default_output_file = fs::path(Slic3r::fold_utf8_to_ascii(default_output_file.string()));
    AppConfig 				&appconfig 				 = *wxGetApp().app_config;
    RemovableDriveManager 	&removable_drive_manager = *wxGetApp().removable_drive_manager();
    // Get a last save path, either to removable media or to an internal media.
    std::string      		 start_dir 				 = appconfig.get_last_output_dir(default_output_file.parent_path().string(), prefer_removable);
    if (prefer_removable) {
        // Returns a path to a removable media if it exists, prefering start_dir. Update the internal removable drives database.
        start_dir = removable_drive_manager.get_removable_drive_path(start_dir);
        if (start_dir.empty())
            // Direct user to the last internal media.
            start_dir = appconfig.get_last_output_dir(default_output_file.parent_path().string(), false);
    }

    fs::path output_path;
    {
        std::string ext = default_output_file.extension().string();
        wxFileDialog dlg(this, (printer_technology() == ptFFF) ? _L("Save G-code file as:") : _L("Save SLA file as:"),
            start_dir,
            from_path(default_output_file.filename()),
            GUI::file_wildcards((printer_technology() == ptFFF) ? FT_GCODE : FT_SL1, ext),
            wxFD_SAVE | wxFD_OVERWRITE_PROMPT
        );
        if (dlg.ShowModal() == wxID_OK) {
            output_path = into_path(dlg.GetPath());
            while (has_illegal_filename_characters(output_path.filename().string())) {
                show_error(this, _L("The provided file name is not valid.") + "\n" +
                    _L("The following characters are not allowed by a FAT file system:") + " <>:/\\|?*\"");
                dlg.SetFilename(from_path(output_path.filename()));
                if (dlg.ShowModal() == wxID_OK)
                    output_path = into_path(dlg.GetPath());
                else {
                    output_path.clear();
                    break;
                }
            }
        }
    }

    if (! output_path.empty()) {
        bool path_on_removable_media = removable_drive_manager.set_and_verify_last_save_path(output_path.string());
        //bool path_on_removable_media = false;
        p->notification_manager->new_export_began(path_on_removable_media);
        p->exporting_status = path_on_removable_media ? ExportingStatus::EXPORTING_TO_REMOVABLE : ExportingStatus::EXPORTING_TO_LOCAL;
        p->last_output_path = output_path.string();
        p->last_output_dir_path = output_path.parent_path().string();
        p->export_gcode(output_path, path_on_removable_media);
        // Storing a path to AppConfig either as path to removable media or a path to internal media.
        // is_path_on_removable_drive() is called with the "true" parameter to update its internal database as the user may have shuffled the external drives
        // while the dialog was open.
        appconfig.update_last_output_dir(output_path.parent_path().string(), path_on_removable_media);

        try {
            json j;
            auto printer_config = Slic3r::GUI::wxGetApp().preset_bundle->printers.get_edited_preset_with_vendor_profile().preset;
            if (printer_config.is_system) {
                j["printer_preset"] = printer_config.name;
            } else {
                j["printer_preset"] = printer_config.config.opt_string("inherits");
            }

            PresetBundle *preset_bundle = wxGetApp().preset_bundle;
            if (preset_bundle) {
                j["gcode_printer_model"] = preset_bundle->printers.get_edited_preset().get_printer_type(preset_bundle);
            }
            NetworkAgent *agent = wxGetApp().getAgent();
        } catch (...) {}

    }
}

void Plater::send_to_printer(bool isall)
{
    p->on_action_send_to_printer(isall);
}

//BBS export gcode 3mf to file
void Plater::export_gcode_3mf(bool export_all)
{
    if (p->model.objects.empty())
        return;

    if (p->process_completed_with_error == p->partplate_list.get_curr_plate_index())
        return;

    //calc default_output_file, get default output file from background process
    fs::path default_output_file;
    AppConfig& appconfig = *wxGetApp().app_config;
    std::string start_dir;
    try {
        // Update the background processing, so that the placeholder parser will get the correct values for the ouput file template.
        // Also if there is something wrong with the current configuration, a pop-up dialog will be shown and the export will not be performed.
        unsigned int state = this->p->update_restart_background_process(false, false);
        if (state & priv::UPDATE_BACKGROUND_PROCESS_INVALID)
            return;
        default_output_file = this->p->background_process.output_filepath_for_project("");
    }
    catch (const Slic3r::PlaceholderParserError& ex) {
        // Show the error with monospaced font.
        show_error(this, ex.what(), true);
        return;
    }
    catch (const std::exception& ex) {
        show_error(this, ex.what(), false);
        return;
    }
    default_output_file.replace_extension(".gcode.3mf");
    default_output_file = fs::path(Slic3r::fold_utf8_to_ascii(default_output_file.string()));

    //Get a last save path
    start_dir = appconfig.get_last_output_dir(default_output_file.parent_path().string(), false);

    fs::path output_path;
    {
        std::string ext = default_output_file.extension().string();
        wxFileDialog dlg(this, _L("Save Sliced file as:"),
            start_dir,
            from_path(default_output_file.filename()),
            GUI::file_wildcards(FT_GCODE_3MF, ""),
            wxFD_SAVE | wxFD_OVERWRITE_PROMPT
        );
        if (dlg.ShowModal() == wxID_OK) {
            output_path = into_path(dlg.GetPath());
            ext = output_path.extension().string();
            if (ext != ".3mf")
                output_path = output_path.string() + ".3mf";
        }
    }

    if (!output_path.empty()) {
        //BBS do not set to removable media path
        bool path_on_removable_media = false;
        p->notification_manager->new_export_began(path_on_removable_media);
        p->exporting_status = path_on_removable_media ? ExportingStatus::EXPORTING_TO_REMOVABLE : ExportingStatus::EXPORTING_TO_LOCAL;
        //BBS do not save last output path
        p->last_output_path = output_path.string();
        p->last_output_dir_path = output_path.parent_path().string();
        int plate_idx = get_partplate_list().get_curr_plate_index();
        if (export_all)
            plate_idx = PLATE_ALL_IDX;
        export_3mf(output_path, SaveStrategy::Silence | SaveStrategy::SplitModel | SaveStrategy::WithGcode | SaveStrategy::SkipModel, plate_idx); // BBS: silence

        RemovableDriveManager& removable_drive_manager = *wxGetApp().removable_drive_manager();


        bool on_removable = removable_drive_manager.is_path_on_removable_drive(p->last_output_dir_path);


        // update last output dir
        appconfig.update_last_output_dir(output_path.parent_path().string(), false);
        p->notification_manager->push_exporting_finished_notification(output_path.string(), p->last_output_dir_path, on_removable);
    }
}

void Plater::send_gcode_finish(wxString name)
{
    auto out_str = GUI::format(_L("The file %s has been sent to the printer's storage space and can be viewed on the printer."), name);
    p->notification_manager->push_exporting_finished_notification(out_str, "", false);
}

void Plater::export_core_3mf()
{
    wxString path = p->get_export_file(FT_3MF);
    if (path.empty()) { return; }
    const std::string path_u8 = into_u8(path);
    export_3mf(path_u8, SaveStrategy::Silence);
}

// Following lambda generates a combined mesh for export with normals pointing outwards.
TriangleMesh Plater::combine_mesh_fff(const ModelObject& mo, int instance_id, std::function<void(const std::string&)> notify_func)
{
    TriangleMesh mesh;

    std::vector<csg::CSGPart> csgmesh;
    csgmesh.reserve(2 * mo.volumes.size());
    bool has_splitable_volume = csg::model_to_csgmesh(mo, Transform3d::Identity(), std::back_inserter(csgmesh),
        csg::mpartsPositive | csg::mpartsNegative);
        
    std::string fail_msg = _u8L("Unable to perform boolean operation on model meshes. "
        "Only positive parts will be kept. You may fix the meshes and try again.");
    if (auto fail_reason_name = csg::check_csgmesh_booleans(Range{ std::begin(csgmesh), std::end(csgmesh) }); std::get<0>(fail_reason_name) != csg::BooleanFailReason::OK) {
        std::string name = std::get<1>(fail_reason_name);
        std::map<csg::BooleanFailReason, std::string> fail_reasons = {
            {csg::BooleanFailReason::OK, "OK"},
            {csg::BooleanFailReason::MeshEmpty, Slic3r::format( _u8L("Reason: part \"%1%\" is empty."), name)},
            {csg::BooleanFailReason::NotBoundAVolume, Slic3r::format(_u8L("Reason: part \"%1%\" does not bound a volume."), name)},
            {csg::BooleanFailReason::SelfIntersect, Slic3r::format(_u8L("Reason: part \"%1%\" has self intersection."), name)},
            {csg::BooleanFailReason::NoIntersection, Slic3r::format(_u8L("Reason: \"%1%\" and another part have no intersection."), name)} };
        fail_msg += " " + fail_reasons[std::get<0>(fail_reason_name)];
    }
    else {
        try {
            MeshBoolean::mcut::McutMeshPtr meshPtr = csg::perform_csgmesh_booleans_mcut(Range{ std::begin(csgmesh), std::end(csgmesh) });
            mesh = MeshBoolean::mcut::mcut_to_triangle_mesh(*meshPtr);
        }
        catch (...) {}
#if 0
        // if mcut fails, try again with CGAL
        if (mesh.empty()) {
            try {
                auto meshPtr = csg::perform_csgmesh_booleans(Range{ std::begin(csgmesh), std::end(csgmesh) });
                mesh = MeshBoolean::cgal::cgal_to_triangle_mesh(*meshPtr);
                }
            catch (...) {}
        }
#endif
    }

    if (mesh.empty()) {
        if (notify_func)
            notify_func(fail_msg);

        for (const ModelVolume* v : mo.volumes)
            if (v->is_model_part()) {
                TriangleMesh vol_mesh(v->mesh());
                vol_mesh.transform(v->get_matrix(), true);
                mesh.merge(vol_mesh);
            }
    }

    if (instance_id == -1) {
        TriangleMesh vols_mesh(mesh);
        mesh = TriangleMesh();
        for (const ModelInstance* i : mo.instances) {
            TriangleMesh m = vols_mesh;
            m.transform(i->get_matrix(), true);
            mesh.merge(m);
        }
    }
    else if (0 <= instance_id && instance_id < int(mo.instances.size()))
        mesh.transform(mo.instances[instance_id]->get_matrix(), true);
    return mesh;
}

// BBS export with/without boolean, however, stil merge mesh
#define EXPORT_WITH_BOOLEAN 0
void Plater::export_stl(bool extended, bool selection_only, bool multi_stls)
{
    if (p->model.objects.empty()) { return; }

    wxString path;
    if (multi_stls) {
        wxDirDialog dlg(this, _L("Choose a directory"), from_u8(wxGetApp().app_config->get_last_dir()),
                        wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
        if (dlg.ShowModal() == wxID_OK) {
            path = dlg.GetPath() + "/";
        }
    } else {
        path = p->get_export_file(FT_STL);
    }
    if (path.empty()) { return; }
    const std::string path_u8 = into_u8(path);

    wxBusyCursor wait;
    const auto& selection = p->get_selection();
    const auto obj_idx = selection.get_object_idx();

#if EXPORT_WITH_BOOLEAN
    if (selection_only && (obj_idx == -1 || selection.is_wipe_tower()))
        return;
#else
    // BBS support selecting multiple objects
    if (selection_only && selection.is_wipe_tower()) return;

    // BBS
    if (selection_only) {
        // only support selection single full object and mulitiple full object
        if (!selection.is_single_full_object() && !selection.is_multiple_full_object()) return;
    }

    // Following lambda generates a combined mesh for export with normals pointing outwards.
    auto mesh_to_export_fff_no_boolean = [this](const ModelObject &mo, int instance_id) {
        TriangleMesh mesh;

        //Prusa export negative parts
        std::vector<csg::CSGPart> csgmesh;
        csgmesh.reserve(2 * mo.volumes.size());
        csg::model_to_csgmesh(mo, Transform3d::Identity(), std::back_inserter(csgmesh),
                              csg::mpartsPositive | csg::mpartsNegative | csg::mpartsDoSplits);

        auto csgrange = range(csgmesh);
        if (csg::is_all_positive(csgrange)) {
            mesh = TriangleMesh{csg::csgmesh_merge_positive_parts(csgrange)};
        } else if (std::get<2>(csg::check_csgmesh_booleans(csgrange)) == csgrange.end()) {
            try {
                auto cgalm = csg::perform_csgmesh_booleans(csgrange);
                mesh = MeshBoolean::cgal::cgal_to_triangle_mesh(*cgalm);
            } catch (...) {}
        }

        if (mesh.empty()) {
            get_notification_manager()->push_plater_error_notification(
                _u8L("Unable to perform boolean operation on model meshes. "
                     "Only positive parts will be exported."));

            for (const ModelVolume* v : mo.volumes)
                if (v->is_model_part()) {
                    TriangleMesh vol_mesh(v->mesh());
                    vol_mesh.transform(v->get_matrix(), true);
                    mesh.merge(vol_mesh);
                }
        }
        if (instance_id == -1) {
            TriangleMesh vols_mesh(mesh);
            mesh = TriangleMesh();
            for (const ModelInstance *i : mo.instances) {
                TriangleMesh m = vols_mesh;
                m.transform(i->get_matrix(), true);
                mesh.merge(m);
            }
        } else if (0 <= instance_id && instance_id < int(mo.instances.size()))
            mesh.transform(mo.instances[instance_id]->get_matrix(), true);
        return mesh;
    };
#endif
    auto mesh_to_export_sla = [&, this](const ModelObject& mo, int instance_id) {
        TriangleMesh mesh;

        const SLAPrintObject *object = this->p->sla_print.get_print_object_by_model_object_id(mo.id());

        if (auto m = object->get_mesh_to_print(); m.empty())
            mesh = combine_mesh_fff(mo, instance_id, [this](const std::string& msg) {return get_notification_manager()->push_plater_error_notification(msg); });
        else {
            const Transform3d mesh_trafo_inv = object->trafo().inverse();
            const bool is_left_handed = object->is_left_handed();

            auto pad_mesh = extended? object->pad_mesh() : TriangleMesh{};
            pad_mesh.transform(mesh_trafo_inv);

            auto supports_mesh = extended ? object->support_mesh() : TriangleMesh{};
            supports_mesh.transform(mesh_trafo_inv);

            const std::vector<SLAPrintObject::Instance>& obj_instances = object->instances();
            for (const SLAPrintObject::Instance& obj_instance : obj_instances) {
                auto it = std::find_if(object->model_object()->instances.begin(), object->model_object()->instances.end(),
                                       [&obj_instance](const ModelInstance *mi) { return mi->id() == obj_instance.instance_id; });
                assert(it != object->model_object()->instances.end());

                if (it != object->model_object()->instances.end()) {
                    const bool one_inst_only = selection_only && ! selection.is_single_full_object();

                    const int instance_idx = it - object->model_object()->instances.begin();
                    const Transform3d& inst_transform = one_inst_only
                                                            ? Transform3d::Identity()
                                                            : object->model_object()->instances[instance_idx]->get_transformation().get_matrix();

                    TriangleMesh inst_mesh;

                    if (!pad_mesh.empty()) {
                        TriangleMesh inst_pad_mesh = pad_mesh;
                        inst_pad_mesh.transform(inst_transform, is_left_handed);
                        inst_mesh.merge(inst_pad_mesh);
                    }

                    if (!supports_mesh.empty()) {
                        TriangleMesh inst_supports_mesh = supports_mesh;
                        inst_supports_mesh.transform(inst_transform, is_left_handed);
                        inst_mesh.merge(inst_supports_mesh);
                    }

                    TriangleMesh inst_object_mesh = object->get_mesh_to_print();

                    inst_object_mesh.transform(mesh_trafo_inv);
                    inst_object_mesh.transform(inst_transform, is_left_handed);

                    inst_mesh.merge(inst_object_mesh);

                           // ensure that the instance lays on the bed
                    inst_mesh.translate(0.0f, 0.0f, -inst_mesh.bounding_box().min.z());

                           // merge instance with global mesh
                    mesh.merge(inst_mesh);

                    if (one_inst_only)
                        break;
                }
            }
        }

        return mesh;
    };

    std::function<TriangleMesh(const ModelObject& mo, int instance_id)>
        mesh_to_export;

    if (p->printer_technology == ptFFF)
#if EXPORT_WITH_BOOLEAN
        mesh_to_export = [this](const ModelObject& mo, int instance_id) {return Plater::combine_mesh_fff(mo, instance_id,
            [this](const std::string& msg) {return get_notification_manager()->push_plater_error_notification(msg); }); };
#else
        mesh_to_export = mesh_to_export_fff_no_boolean;
#endif
    else
        mesh_to_export = mesh_to_export_sla;

    auto get_save_file = [](std::string const & dir, std::string const & name) {
        auto path = dir + name + ".stl";
        int n = 1;
        while (boost::filesystem::exists(path))
            path = dir + name + "(" + std::to_string(n++) + ").stl";
        return path;
    };

    TriangleMesh mesh;
    if (selection_only) {
        if (selection.is_single_full_object()) {
            const auto obj_idx = selection.get_object_idx();
            const ModelObject* model_object = p->model.objects[obj_idx];
            if (selection.get_mode() == Selection::Instance)
                mesh = mesh_to_export(*model_object, (model_object->instances.size() > 1) ? -1 : selection.get_instance_idx());
            else {
                const GLVolume* volume = selection.get_first_volume();
                mesh = model_object->volumes[volume->volume_idx()]->mesh();
                mesh.transform(volume->get_volume_transformation().get_matrix(), true);
            }

            if (model_object->instances.size() == 1) mesh.translate(-model_object->origin_translation.cast<float>());
        }
        else if (selection.is_multiple_full_object() && !multi_stls) {
            const std::set<std::pair<int, int>>& instances_idxs = p->get_selection().get_selected_object_instances();
            for (const std::pair<int, int>& i : instances_idxs) {
                ModelObject* object = p->model.objects[i.first];
                mesh.merge(mesh_to_export(*object, i.second));
            }
        }
        else if (selection.is_multiple_full_object() && multi_stls) {
            const std::set<std::pair<int, int>> &instances_idxs = p->get_selection().get_selected_object_instances();
            for (const std::pair<int, int> &i : instances_idxs) {
                ModelObject *object = p->model.objects[i.first];
                auto mesh = mesh_to_export(*object, i.second);
                mesh.translate(-object->origin_translation.cast<float>());

                Slic3r::store_stl(get_save_file(path_u8, object->name).c_str(), &mesh, true);
            }
            return;
        }
    }
    else if (!multi_stls) {
        for (const ModelObject* o : p->model.objects) {
            mesh.merge(mesh_to_export(*o, -1));
        }
    } else {
        for (const ModelObject* o : p->model.objects) {
            auto mesh = mesh_to_export(*o, -1);
            mesh.translate(-o->origin_translation.cast<float>());
            Slic3r::store_stl(get_save_file(path_u8, o->name).c_str(), &mesh, true);
        }
        return;
    }

    Slic3r::store_stl(path_u8.c_str(), &mesh, true);
}

//BBS: remove amf export
/*void Plater::export_amf()
{
    if (p->model.objects.empty()) { return; }

    wxString path = p->get_export_file(FT_AMF);
    if (path.empty()) { return; }
    const std::string path_u8 = into_u8(path);

    wxBusyCursor wait;
    bool export_config = true;
    DynamicPrintConfig cfg = wxGetApp().preset_bundle->full_config_secure();
    bool full_pathnames = false;
    if (Slic3r::store_amf(path_u8.c_str(), &p->model, export_config ? &cfg : nullptr, full_pathnames)) {
        ; //store success
    } else {
        ; // store failed
    }
}*/

namespace {
std::string get_file_name(const std::string &file_path)
{
    size_t pos_last_delimiter = file_path.find_last_of("/\\");
    size_t pos_point          = file_path.find_last_of('.');
    size_t offset             = pos_last_delimiter + 1;
    size_t count              = pos_point - pos_last_delimiter - 1;
    return file_path.substr(offset, count);
}
using SvgFile = EmbossShape::SvgFile;
using SvgFiles = std::vector<SvgFile*>;
std::string create_unique_3mf_filepath(const std::string &file, const SvgFiles svgs)
{
    // const std::string MODEL_FOLDER = "3D/"; // copy from file 3mf.cpp
    std::string path_in_3mf = "3D/" + file + ".svg";
    size_t suffix_number = 0;
    bool is_unique = false;
    do{
        is_unique = true;
        path_in_3mf = "3D/" + file + ((suffix_number++)? ("_" + std::to_string(suffix_number)) : "") + ".svg";
        for (SvgFile *svgfile : svgs) {
            if (svgfile->path_in_3mf.empty())
                continue;
            if (svgfile->path_in_3mf.compare(path_in_3mf) == 0) {
                is_unique = false;
                break;
            }
        } 
    } while (!is_unique);
    return path_in_3mf;
}

bool set_by_local_path(SvgFile &svg, const SvgFiles& svgs)
{
    // Try to find already used svg file
    for (SvgFile *svg_ : svgs) {
        if (svg_->path_in_3mf.empty())
            continue;
        if (svg.path.compare(svg_->path) == 0) {
            svg.path_in_3mf = svg_->path_in_3mf;
            return true;
        }
    }
    return false;
}

/// <summary>
/// Function to secure private data before store to 3mf
/// </summary>
/// <param name="model">Data(also private) to clean before publishing</param>
void publish(Model &model, SaveStrategy strategy) {

    // SVG file publishing
    bool exist_new = false;
    SvgFiles svgfiles;
    for (ModelObject *object: model.objects){
        for (ModelVolume *volume : object->volumes) {
            if (!volume->emboss_shape.has_value())
                continue;
            if (volume->text_configuration.has_value())
                continue; // text dosen't have svg path

            assert(volume->emboss_shape->svg_file.has_value());
            if (!volume->emboss_shape->svg_file.has_value())
                continue;

            SvgFile* svg = &(*volume->emboss_shape->svg_file);
            if (svg->path_in_3mf.empty())
                exist_new = true;
            svgfiles.push_back(svg);
        }
    }

    // Orca: don't show this in silence mode
    if (exist_new && !(strategy & SaveStrategy::Silence)) {
        MessageDialog dialog(nullptr,
                             _L("Are you sure you want to store original SVGs with their local paths into the 3MF file?\n"
                                "If you hit 'NO', all SVGs in the project will not be editable any more."),
                             _L("Private protection"), wxYES_NO | wxICON_QUESTION);
        if (dialog.ShowModal() == wxID_NO){
            for (ModelObject *object : model.objects) 
                for (ModelVolume *volume : object->volumes)
                    if (volume->emboss_shape.has_value())
                        volume->emboss_shape.reset();
        }
    }

    for (SvgFile* svgfile : svgfiles){
        if (!svgfile->path_in_3mf.empty())
            continue; // already suggested path (previous save)

        // create unique name for svgs, when local path differ
        std::string filename = "unknown";
        if (!svgfile->path.empty()) {
            if (set_by_local_path(*svgfile, svgfiles))
                continue;
            // check whether original filename is already in:
            filename = get_file_name(svgfile->path);
        }
        svgfile->path_in_3mf = create_unique_3mf_filepath(filename, svgfiles);        
    }
}
}

// BBS: backup
int Plater::export_3mf(const boost::filesystem::path& output_path, SaveStrategy strategy, int export_plate_idx, Export3mfProgressFn proFn)
{
    int ret = 0;
    //if (p->model.objects.empty()) {
    //    MessageDialog dialog(nullptr, _L("No objects to export."), _L("Save project"), wxYES);
    //    if (dialog.ShowModal() == wxYES)
    //        return -1;
    //}

    if (output_path.empty())
        return -1;

    bool export_config = true;
    wxString path = from_path(output_path);

    if (!path.Lower().EndsWith(".3mf"))
        return -1;

    // take care about private data stored into .3mf
    // modify model
    publish(p->model, strategy);

    DynamicPrintConfig cfg = wxGetApp().preset_bundle->full_config_secure();
    const std::string path_u8 = into_u8(path);
    wxBusyCursor wait;

    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << boost::format(": path=%1%, backup=%2%, export_plate_idx=%3%, SaveStrategy=%4%")
        %output_path.string()%(strategy & SaveStrategy::Backup)%export_plate_idx %(unsigned int)strategy;

    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": path=%1%, backup=%2%, export_plate_idx=%3%, SaveStrategy=%4%")
        % std::string("") % (strategy & SaveStrategy::Backup) % export_plate_idx % (unsigned int)strategy;

    //BBS: add plate logic for thumbnail generate
    std::vector<ThumbnailData*> thumbnails;
    std::vector<ThumbnailData*> no_light_thumbnails;
    std::vector<ThumbnailData*> calibration_thumbnails;
    std::vector<ThumbnailData*> top_thumbnails;
    std::vector<ThumbnailData*> picking_thumbnails;
    std::vector<PlateBBoxData*> plate_bboxes;
    // BBS: backup
    if (!(strategy & SaveStrategy::Backup)) {
        for (int i = 0; i < p->partplate_list.get_plate_count(); i++) {
            ThumbnailData* thumbnail_data = &p->partplate_list.get_plate(i)->thumbnail_data;
            if (p->partplate_list.get_plate(i)->thumbnail_data.is_valid() &&  using_exported_file()) {
                //no need to generate thumbnail
                BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": non need to re-generate thumbnail for gcode/exported mode of plate %1%")%i;
            }
            else {
                BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": re-generate thumbnail for plate %1%") % i;
                const ThumbnailsParams thumbnail_params = { {}, false, true, true, true, i };
                p->generate_thumbnail(p->partplate_list.get_plate(i)->thumbnail_data, THUMBNAIL_SIZE_3MF.first, THUMBNAIL_SIZE_3MF.second,
                                    thumbnail_params, Camera::EType::Ortho);
            }
            thumbnails.push_back(thumbnail_data);

            ThumbnailData *no_light_thumbnail_data = &p->partplate_list.get_plate(i)->no_light_thumbnail_data;
            if (p->partplate_list.get_plate(i)->no_light_thumbnail_data.is_valid() && using_exported_file()) {
                // no need to generate thumbnail
                BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": non need to re-generate thumbnail for gcode/exported mode of plate %1%") % i;
            } else {
                BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": re-generate thumbnail for plate %1%") % i;
                const ThumbnailsParams thumbnail_params = {{}, false, true, true, true, i};
                p->generate_thumbnail(p->partplate_list.get_plate(i)->no_light_thumbnail_data, THUMBNAIL_SIZE_3MF.first, THUMBNAIL_SIZE_3MF.second,
                    thumbnail_params, Camera::EType::Ortho,false,false,true);
            }
            no_light_thumbnails.push_back(no_light_thumbnail_data);
            //ThumbnailData* calibration_data = &p->partplate_list.get_plate(i)->cali_thumbnail_data;
            //calibration_thumbnails.push_back(calibration_data);
            PlateBBoxData* plate_bbox_data = &p->partplate_list.get_plate(i)->cali_bboxes_data;
            plate_bboxes.push_back(plate_bbox_data);

            //generate top and picking thumbnails
            ThumbnailData* top_thumbnail = &p->partplate_list.get_plate(i)->top_thumbnail_data;
            if (top_thumbnail->is_valid() &&  using_exported_file()) {
                //no need to generate thumbnail
                BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": non need to re-generate top_thumbnail for gcode/exported mode of plate %1%")%i;
            }
            else {
                BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": re-generate top_thumbnail for plate %1%") % i;
                const ThumbnailsParams thumbnail_params = { {}, false, true, false, true, i };
                p->generate_thumbnail(p->partplate_list.get_plate(i)->top_thumbnail_data, THUMBNAIL_SIZE_3MF.first, THUMBNAIL_SIZE_3MF.second,
                                    thumbnail_params, Camera::EType::Ortho, true, false);
            }
            top_thumbnails.push_back(top_thumbnail);

            ThumbnailData* picking_thumbnail = &p->partplate_list.get_plate(i)->pick_thumbnail_data;
            if (picking_thumbnail->is_valid() &&  using_exported_file()) {
                //no need to generate thumbnail
                BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": non need to re-generate pick_thumbnail for gcode/exported mode of plate %1%")%i;
            }
            else {
                BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": re-generate pick_thumbnail for plate %1%") % i;
                const ThumbnailsParams thumbnail_params = { {}, false, true, false, true, i };
                p->generate_thumbnail(p->partplate_list.get_plate(i)->pick_thumbnail_data, THUMBNAIL_SIZE_3MF.first, THUMBNAIL_SIZE_3MF.second,
                                    thumbnail_params, Camera::EType::Ortho, true, true);
            }
            picking_thumbnails.push_back(picking_thumbnail);
        }

        if (p->partplate_list.get_curr_plate()->is_slice_result_valid()) {
            //BBS generate BBS calibration thumbnails
            int index = p->partplate_list.get_curr_plate_index();
            //ThumbnailData* calibration_data = calibration_thumbnails[index];
            //const ThumbnailsParams calibration_params = { {}, false, true, true, true, p->partplate_list.get_curr_plate_index() };
            //p->generate_calibration_thumbnail(*calibration_data, PartPlate::cali_thumbnail_width, PartPlate::cali_thumbnail_height, calibration_params);
            if (using_exported_file()) {
                //do nothing
            }
            else
                *plate_bboxes[index] = p->generate_first_layer_bbox();
        }
    }

    //BBS: add bbs 3mf logic
    PlateDataPtrs plate_data_list;
    p->partplate_list.store_to_3mf_structure(plate_data_list, (strategy & SaveStrategy::WithGcode || strategy & SaveStrategy::WithSliceInfo), export_plate_idx);

    // BBS: backup
    PresetBundle& preset_bundle = *wxGetApp().preset_bundle;
    std::vector<Preset*> project_presets = preset_bundle.get_current_project_embedded_presets();

    StoreParams store_params;
    store_params.path  = path_u8.c_str();
    store_params.model = &p->model;
    store_params.plate_data_list = plate_data_list;
    store_params.export_plate_idx = export_plate_idx;
    store_params.project_presets = project_presets;
    store_params.config = export_config ? &cfg : nullptr;
    store_params.thumbnail_data = thumbnails;
    store_params.no_light_thumbnail_data  = no_light_thumbnails;
    store_params.top_thumbnail_data = top_thumbnails;
    store_params.pick_thumbnail_data = picking_thumbnails;
    store_params.calibration_thumbnail_data = calibration_thumbnails;
    store_params.proFn = proFn;
    store_params.id_bboxes = plate_bboxes;//BBS
    store_params.project = &p->project;
    store_params.strategy = strategy | SaveStrategy::Zip64;


    // get type and color for platedata
    auto* filament_color = dynamic_cast<const ConfigOptionStrings*>(cfg.option("filament_colour"));
    auto* nozzle_diameter_option = dynamic_cast<const ConfigOptionFloats*>(cfg.option("nozzle_diameter"));
    auto* filament_id_opt = dynamic_cast<const ConfigOptionStrings*>(cfg.option("filament_ids"));
    std::string nozzle_diameter_str;
    if (nozzle_diameter_option)
        nozzle_diameter_str = nozzle_diameter_option->serialize();

    std::string printer_model_id = preset_bundle.printers.get_edited_preset().get_printer_type(&preset_bundle);

    for (int i = 0; i < plate_data_list.size(); i++) {
        PlateData *plate_data = plate_data_list[i];
        plate_data->printer_model_id = printer_model_id;
        plate_data->nozzle_diameters = nozzle_diameter_str;
        for (auto it = plate_data->slice_filaments_info.begin(); it != plate_data->slice_filaments_info.end(); it++) {
            std::string display_filament_type;
            it->type  = cfg.get_filament_type(display_filament_type, it->id);
            it->filament_id = filament_id_opt ? filament_id_opt->get_at(it->id) : "";
            it->color = filament_color ? filament_color->get_at(it->id) : "#FFFFFF";
            // save filament info used in curr plate
            int index = p->partplate_list.get_curr_plate_index();
            if (store_params.id_bboxes.size() > index) {
                store_params.id_bboxes[index]->filament_ids.push_back(it->id);
                store_params.id_bboxes[index]->filament_colors.push_back(it->color);
            }
        }
    }

    // handle Design Info
    bool has_design_info = false;
    ModelDesignInfo designInfo;
    if (p->model.design_info != nullptr) {
        if (!p->model.design_info->Designer.empty()) {
            BOOST_LOG_TRIVIAL(trace) << "design_info, found designer = " << p->model.design_info->Designer;
            has_design_info = true;
        }
    }
    if (!has_design_info) {
        // add Designed Info
        if (p->model.design_info == nullptr) {
            // set designInfo before export and reset after export
            if (wxGetApp().is_user_login()) {
                p->model.design_info                 = std::make_shared<ModelDesignInfo>();
                //p->model.design_info->Designer       = wxGetApp().getAgent()->get_user_nickanme();
                p->model.design_info->Designer       = "";
                p->model.design_info->DesignerUserId = wxGetApp().getAgent()->get_user_id();
                BOOST_LOG_TRIVIAL(trace) << "design_info prepare, designer = "<< "";
                BOOST_LOG_TRIVIAL(trace) << "design_info prepare, designer_user_id = " << p->model.design_info->DesignerUserId;
            }
        }
    }

    bool store_result = Slic3r::store_bbs_3mf(store_params);
    // reset designed info
    if (!has_design_info)
        p->model.design_info = nullptr;

    if (store_result) {
        if (!(store_params.strategy & SaveStrategy::Silence)) {
            // Success
            p->set_project_filename(path);
            BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << __LINE__ << " call set_project_filename: " << path;
        }
    }
    else {
        ret = -1;
    }

    if (project_presets.size() > 0)
    {
        for (unsigned int i = 0; i < project_presets.size(); i++)
        {
            delete project_presets[i];
        }
        project_presets.clear();
    }

    release_PlateData_list(plate_data_list);

    for (unsigned int i = 0; i < calibration_thumbnails.size(); i++)
    {
        //release the data here, as it will always be generated when export
        calibration_thumbnails[i]->reset();
    }
    for (unsigned int i = 0; i < no_light_thumbnails.size(); i++) {
        // release the data here, as it will always be generated when export
        no_light_thumbnails[i]->reset();
    }
    for (unsigned int i = 0; i < top_thumbnails.size(); i++)
    {
        //release the data here, as it will always be generated when export
        top_thumbnails[i]->reset();
    }
    top_thumbnails.clear();
    for (unsigned int i = 0; i < picking_thumbnails.size(); i++)
    {
        //release the data here, as it will always be generated when export
        picking_thumbnails[i]->reset();;
    }
    picking_thumbnails.clear();

    return ret;
}

void Plater::publish_project()
{
    return;
}


void Plater::reload_from_disk()
{
    p->reload_from_disk();
}

void Plater::replace_with_stl()
{
    p->replace_with_stl();
}

void Plater::reload_all_from_disk()
{
    p->reload_all_from_disk();
}

bool Plater::has_toolpaths_to_export() const
{
    return  p->preview->get_canvas3d()->has_toolpaths_to_export();
}

void Plater::export_toolpaths_to_obj() const
{
    if ((printer_technology() != ptFFF) || !is_preview_loaded())
        return;

    wxString path = p->get_export_file(FT_OBJ);
    if (path.empty())
        return;

    wxBusyCursor wait;
    p->preview->get_canvas3d()->export_toolpaths_to_obj(into_u8(path).c_str());
}

//BBS: add multiple plate reslice logic
bool Plater::reslice()
{
    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(", Line %1%: enter, process_completed_with_error=%2%")%__LINE__ %p->process_completed_with_error;
    // There is "invalid data" button instead "slice now"
    if (p->process_completed_with_error == p->partplate_list.get_curr_plate_index())
    {
        BOOST_LOG_TRIVIAL(warning) << __FUNCTION__ << boost::format(": process_completed_with_error, return directly");
        reset_gcode_toolpaths();
        return true;
    }

    // In case SLA gizmo is in editing mode, refuse to continue
    // and notify user that he should leave it first.
    if (get_view3D_canvas3D()->get_gizmos_manager().is_in_editing_mode(true))
        return true;
    
    // Stop the running (and queued) UI jobs and only proceed if they actually
    // get stopped.
    unsigned timeout_ms = 10000;
    if (!stop_queue(this->get_ui_job_worker(), timeout_ms)) {
        BOOST_LOG_TRIVIAL(error) << "Could not stop UI job within "
                                 << timeout_ms << " milliseconds timeout!";
        return true;
    }

    // Orca: regenerate CalibPressureAdvancePattern custom G-code to apply changes
    if (model().calib_pa_pattern) {
        _calib_pa_pattern_gen_gcode();
    }

    if (printer_technology() == ptSLA) {
        for (auto& object : model().objects)
            if (object->sla_points_status == sla::PointsStatus::NoPoints)
                object->sla_points_status = sla::PointsStatus::Generating;
    }

    //FIXME Don't reslice if export of G-code or sending to OctoPrint is running.
    // bitmask of UpdateBackgroundProcessReturnState
    unsigned int state = this->p->update_background_process(true);
    if (state & priv::UPDATE_BACKGROUND_PROCESS_REFRESH_SCENE)
        this->p->view3D->reload_scene(false);
    // If the SLA processing of just a single object's supports is running, restart slicing for the whole object.
    if (printer_technology() == ptFFF && p->has_incompatible_mixed_filament_in_use()) {
        BOOST_LOG_TRIVIAL(warning) << __FUNCTION__ << ": incompatible mixed filament in use, blocking slice";
        p->notification_manager->push_notification(
            NotificationType::FilamentIncompatibleMixed,
            NotificationManager::NotificationLevel::ErrorNotificationLevel,
            into_u8(_L("Mixed filaments contain incompatible material types. Please correct the mixed filaments settings before slicing.")));
        reset_gcode_toolpaths();
        return true;
    }

    // Runtime memory guard: register a callback that fires DURING slicing
    // when available physical memory drops below the threshold (PrintBase.hpp).
    // The guard checks every 500ms at the 138 throw_if_canceled() checkpoints.
    p->preview->set_skip_toolpath_preview(false);
    if (printer_technology() == ptFFF) {
        Print* print_ptr = p->background_process.fff_print();
        if (print_ptr) {
            print_ptr->set_memory_guard_callback([this]() -> bool {
                auto promise = std::make_shared<std::promise<bool>>();
                auto future  = promise->get_future();

                this->CallAfter([this, promise]() {
                    wxString msg = _L("Available system memory is critically low during slicing. "
                                      "Continuing may cause the application to freeze or crash.")
                        + "\n\n"
                        + _L("Do you want to continue slicing?")
                        + "\n\n"
                        + "\n- "
                        + _L("Select \"Yes\" to attempt slicing, but the software may lag or freeze.")
                        + "\n- "
                        + _L("Select \"No\" to terminate the slicing task immediately.");
                    RichMessageDialog dlg(this, msg,
                        _L("Memory Usage Warning"), wxYES_NO | wxNO_DEFAULT | wxICON_WARNING);
                    dlg.SetYesNoLabels(_L("Yes, Continue"), _L("No, Stop"));

                    bool result = (dlg.ShowModal() == wxID_YES);
                    if (result) {
                        // Skip toolpath preview to reduce memory usage on
                        // the subsequent load_toolpaths / load_shells phase.
                        this->p->preview->set_skip_toolpath_preview(true);
                    } else {
                        // User chose to cancel: aggressively free the partial
                        // slicing data to reclaim memory before the preview
                        // page loads any rendering buffers.
                        this->p->preview->set_skip_toolpath_preview(true);
                        GCodeProcessorResult* gcode_res = this->p->preview->get_gcode_result();
                        if (gcode_res != nullptr) {
                            gcode_res->moves.clear();
                            gcode_res->moves.shrink_to_fit();
                        }
                    }
                    promise->set_value(result);
                });

                return future.get();
            });
        }
    }

    this->p->background_process.set_task(PrintBase::TaskParams());
    // Only restarts if the state is valid.
    //BBS: jusdge the result
    bool result = this->p->restart_background_process(state | priv::UPDATE_BACKGROUND_PROCESS_FORCE_RESTART);
    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(", Line %1%: restart background,state=%2%, result=%3%")%__LINE__%state %result;
    if ((state & priv::UPDATE_BACKGROUND_PROCESS_INVALID) != 0)
    {
        //BBS: add logs
        BOOST_LOG_TRIVIAL(warning) << __FUNCTION__ << boost::format(": state %1% is UPDATE_BACKGROUND_PROCESS_INVALID, can not slice") % state;
        p->update_fff_scene_only_shells();
        return true;
    }

    if ((!result) && p->m_slice_all && (p->m_cur_slice_plate < (p->partplate_list.get_plate_count() - 1)))
    {
        //slice next
        BOOST_LOG_TRIVIAL(warning) << __FUNCTION__ << boost::format(": in slicing all, current plate %1% already sliced, skip to next") % p->m_cur_slice_plate ;
        SlicingProcessCompletedEvent evt(EVT_PROCESS_COMPLETED, 0,
            SlicingProcessCompletedEvent::Finished, nullptr);
        // Post the "complete" callback message, so that it will slice the next plate soon
        wxQueueEvent(this, evt.Clone());
        p->m_is_slicing = true;
        if (p->m_cur_slice_plate == 0)
            reset_gcode_toolpaths();
        return true;
    }

    if (result) {
        p->m_is_slicing = true;
    }

    bool clean_gcode_toolpaths = true;
    // BBS
    if (p->background_process.running())
    {
        //p->ready_to_slice = false;
        p->main_frame->update_slice_print_status(MainFrame::eEventSliceUpdate, false);
        BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": background process is running, m_is_slicing is true");
    }
    else if (!p->background_process.empty() && !p->background_process.idle()) {
        //p->show_action_buttons(true);
        //p->ready_to_slice = true;
        p->main_frame->update_slice_print_status(MainFrame::eEventSliceUpdate, true);
        BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": background process changes to not_idle, set ready_to_slice back to true");
    }
    else {
        //BBS: add reset logic for empty plate
        PartPlate * current_plate = p->background_process.get_current_plate();

        if (!current_plate->has_printable_instances()) {
            clean_gcode_toolpaths = true;
            current_plate->update_slice_result_valid_state(false);
        }
        else {
            clean_gcode_toolpaths = false;
            current_plate->update_slice_result_valid_state(true);
        }
        p->main_frame->update_slice_print_status(MainFrame::eEventSliceUpdate, false);
        BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": background process in idle state, use previous result, clean_gcode_toolpaths=%1%")%clean_gcode_toolpaths;
    }

    if (clean_gcode_toolpaths)
        reset_gcode_toolpaths();

    p->preview->reload_print(!clean_gcode_toolpaths);

    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": finished, started slicing for plate %1%") % p->partplate_list.get_curr_plate_index();

    record_slice_preset("slicing");
    return true;
}

void Plater::record_slice_preset(std::string action)
{
    // record slice preset
    try
    {
        json j;
        auto printer_preset = wxGetApp().preset_bundle->printers.get_edited_preset_with_vendor_profile().preset;
        if (printer_preset.is_system) {
            j["printer_preset_name"] = printer_preset.name;
        }
        else {
            j["printer_preset_name"] = printer_preset.config.opt_string("inherits");
        }
        const t_config_enum_values* keys_map = print_config_def.get("curr_bed_type")->enum_keys_map;
        if (keys_map) {
            for (auto item : *keys_map) {
                if (item.second == wxGetApp().preset_bundle->project_config.opt_enum<BedType>("curr_bed_type")) {
                    j["curr_bed_type"] = item.first;
                    break;
                }
            }
        }
        auto filament_presets = wxGetApp().preset_bundle->filament_presets;
        for (int i = 0; i < filament_presets.size(); ++i) {
            auto filament_preset = wxGetApp().preset_bundle->filaments.find_preset(filament_presets[i]);
            if (filament_preset->is_system) {
                j["filament_preset_" + std::to_string(i)] = filament_preset->name;
            }
            else {
                j["filament_preset_" + std::to_string(i)] = filament_preset->config.opt_string("inherits");
            }
        }

        Preset& print_preset = wxGetApp().preset_bundle->prints.get_edited_preset();
        if (print_preset.is_system) {
            j["process_preset"] = print_preset.name;
        }
        else {
            j["process_preset"] = print_preset.config.opt_string("inherits");
        }
        j["support_type"] = ConfigOptionEnum<SupportType>::get_enum_names().at(print_preset.config.opt_enum<SupportType>("support_type"));
        j["sparse_infill_pattern"] = ConfigOptionEnum<InfillPattern>::get_enum_names().at(print_preset.config.opt_enum<InfillPattern>("sparse_infill_pattern"));
        j["sparse_infill_density"] = print_preset.config.opt<ConfigOptionPercent>("sparse_infill_density")->value;

        j["brim_type"] = ConfigOptionEnum<BrimType>::get_enum_names().at(print_preset.config.opt_enum<BrimType>("brim_type"));
        j["user_mode"] = wxGetApp().get_mode_str();

        if (p->background_process.fff_print()) {
            const DynamicPrintConfig& full_config = p->background_process.fff_print()->full_print_config();
            json values = json::array();
            if (full_config.has("different_settings_to_system")) {
                std::vector<std::string> different_values = full_config.option<ConfigOptionStrings>("different_settings_to_system")->values;
                for (auto& item : different_values) {
                    values.push_back(item);
                }
            }
            j["different_settings_to_system"] = values;
        }

        j["record_event"] = action;
        NetworkAgent* agent = wxGetApp().getAgent();
    }
    catch (...)
    {
        return;
    }
}

//BBS: add project slicing related logic
int Plater::start_next_slice()
{
    // Stop arrange and (or) optimize rotation tasks.
    //this->stop_jobs();

    if (is_plate_blocked_by_filament_temp_mixing(p->partplate_list.get_curr_plate_index())
        || is_plate_blocked_by_cold_plate(p->partplate_list.get_curr_plate_index()))
    {
        sync_filament_temp_mixing_notification();
        sync_cold_plate_notification();
        if (p->m_slice_all)
        {
            SlicingProcessCompletedEvent evt(EVT_PROCESS_COMPLETED, 0,
                    SlicingProcessCompletedEvent::Finished, nullptr);
            wxQueueEvent(this, evt.Clone());
            return 0;
        }
        p->process_completed_with_error = p->partplate_list.get_curr_plate_index();
        return -1;
    }

    // Second blocker: flow_ratio == 0. NOTE: this uses first-blocker
    // early-return - if temp_mixing above already blocked the plate, this
    // branch never runs and only the temp_mixing banner is shown. Blocking
    // itself still takes effect (single-plate returns -1; slice_all posts
    // Finished to skip the current plate). If both banners must be shown
    // simultaneously, hoist sync_flow_ratio_zero_notification() into the
    // temp_mixing branch before its early return.
    if (is_plate_blocked_by_flow_ratio_zero(p->partplate_list.get_curr_plate_index()))
    {
        sync_flow_ratio_zero_notification();
        if (p->m_slice_all)
        {
            SlicingProcessCompletedEvent evt(EVT_PROCESS_COMPLETED, 0,
                    SlicingProcessCompletedEvent::Finished, nullptr);
            wxQueueEvent(this, evt.Clone());
            return 0;
        }
        p->process_completed_with_error = p->partplate_list.get_curr_plate_index();
        return -1;
    }

    //FIXME Don't reslice if export of G-code or sending to OctoPrint is running.
    // bitmask of UpdateBackgroundProcessReturnState
    unsigned int state = this->p->update_background_process(true, false, false);
    if (state & priv::UPDATE_BACKGROUND_PROCESS_REFRESH_SCENE)
        this->p->view3D->reload_scene(false);

    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": update_background_process returns %1%")%state;
    if (printer_technology() == ptFFF && p->has_incompatible_mixed_filament_in_use()) {
        BOOST_LOG_TRIVIAL(warning) << __FUNCTION__ << ": incompatible mixed filament in use, blocking slice";
        p->notification_manager->push_notification(
            NotificationType::FilamentIncompatibleMixed,
            NotificationManager::NotificationLevel::ErrorNotificationLevel,
            into_u8(_L("Mixed filaments contain incompatible material types. Please correct the mixed filaments settings before slicing.")));

        return -1;
    }
    if (!p->partplate_list.get_curr_plate()->can_slice()) {
        p->process_completed_with_error = p->partplate_list.get_curr_plate_index();
        BOOST_LOG_TRIVIAL(warning) << __FUNCTION__ << boost::format(": found invalidated apply in update_background_process.");
        return -1;
    }

    // Only restarts if the state is valid.
    bool result = this->p->restart_background_process(state | priv::UPDATE_BACKGROUND_PROCESS_FORCE_RESTART);
    if (!result)
    {
        //slice next
        SlicingProcessCompletedEvent evt(EVT_PROCESS_COMPLETED, 0,
                SlicingProcessCompletedEvent::Finished, nullptr);
        // Post the "complete" callback message, so that it will slice the next plate soon
        wxQueueEvent(this, evt.Clone());
    }
    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": restart_background_process returns %1%")%result;

    return 0;
}


void Plater::reslice_SLA_supports(const ModelObject &object, bool postpone_error_messages)
{
    reslice_SLA_until_step(slaposPad, object, postpone_error_messages);
}

void Plater::reslice_SLA_hollowing(const ModelObject &object, bool postpone_error_messages)
{
    reslice_SLA_until_step(slaposDrillHoles, object, postpone_error_messages);
}

void Plater::reslice_SLA_until_step(SLAPrintObjectStep step, const ModelObject &object, bool postpone_error_messages)
{
    //FIXME Don't reslice if export of G-code or sending to OctoPrint is running.
    // bitmask of UpdateBackgroundProcessReturnState
    unsigned int state = this->p->update_background_process(true, postpone_error_messages);
    if (state & priv::UPDATE_BACKGROUND_PROCESS_REFRESH_SCENE)
        this->p->view3D->reload_scene(false);

    if (this->p->background_process.empty() || (state & priv::UPDATE_BACKGROUND_PROCESS_INVALID))
        // Nothing to do on empty input or invalid configuration.
        return;

    // Limit calculation to the single object only.
    PrintBase::TaskParams task;
    task.single_model_object = object.id();
    // If the background processing is not enabled, calculate supports just for the single instance.
    // Otherwise calculate everything, but start with the provided object.
    if (!this->p->background_processing_enabled()) {
        task.single_model_instance_only = true;
        task.to_object_step = step;
    }
    this->p->background_process.set_task(task);
    // and let the background processing start.
    this->p->restart_background_process(state | priv::UPDATE_BACKGROUND_PROCESS_FORCE_RESTART);
}
void Plater::send_gcode_legacy(int plate_idx, Export3mfProgressFn proFn, bool use_3mf)
{
    // if physical_printer is selected, send gcode for this printer
    // DynamicPrintConfig* physical_printer_config = wxGetApp().preset_bundle->physical_printers.get_selected_printer_config();

    auto prepare_upload_filename_for_dialog = [this, use_3mf](fs::path output_file) {
        output_file = fs::path(Slic3r::fold_utf8_to_ascii(output_file.string()));
        if (use_3mf)
            output_file.replace_extension("3mf");

        PartPlate *current_plate = this->get_partplate_list().get_curr_plate();
        if (current_plate != nullptr) {
            const Print *current_print = current_plate->fff_print();
            if (current_print != nullptr && !current_print->print_statistics().estimated_normal_print_time.empty())
                return fs::path(current_print->print_statistics().finalize_output_path(output_file.string()));
        }

        if (current_plate != nullptr && current_plate->is_slice_result_valid() && current_plate->get_slice_result() != nullptr) {
            const auto &estimated_stats = current_plate->get_slice_result()->print_statistics;
            const float normal_time = estimated_stats.modes[static_cast<size_t>(PrintEstimatedStatistics::ETimeMode::Normal)].time;
            if (normal_time > 0.0f) {
                std::string filename = output_file.string();
                const std::string normal_time_str = short_time(get_time_dhms(normal_time));
                boost::replace_all(filename, "{print_time}", normal_time_str);
                boost::replace_all(filename, "{normal_print_time}", normal_time_str);

                const float silent_time = estimated_stats.modes[static_cast<size_t>(PrintEstimatedStatistics::ETimeMode::Stealth)].time;
                if (silent_time > 0.0f)
                    boost::replace_all(filename, "{silent_print_time}", short_time(get_time_dhms(silent_time)));

                output_file = fs::path(filename);
            }
        }

        return output_file;
    };

    // 校验机型
    auto devices = wxGetApp().app_config->get_devices();
    std::string connect_preset = "";
    for (const auto device : devices) {
        if (device.connected) {
            connect_preset = device.preset_name;
        }
    }

    auto current_preset = wxGetApp().preset_bundle->printers.get_edited_preset();

    bool islegal = true;
    std::string c_preset = "";
    if (current_preset.is_system) {
        c_preset = current_preset.name;
    } else {
        auto base_preset = wxGetApp().preset_bundle->printers.get_preset_base(current_preset);
        c_preset         = base_preset->name;
    }

    c_preset.erase(std::remove(c_preset.begin(), c_preset.end(), '('), c_preset.end());
    c_preset.erase(std::remove(c_preset.begin(), c_preset.end(), ')'), c_preset.end());

    connect_preset.erase(std::remove(connect_preset.begin(), connect_preset.end(), '('), connect_preset.end());
    connect_preset.erase(std::remove(connect_preset.begin(), connect_preset.end(), ')'), connect_preset.end());

    islegal = (c_preset == connect_preset);

    DynamicPrintConfig* physical_printer_config = &Slic3r::GUI::wxGetApp().preset_bundle->printers.get_edited_preset().config;
    if (! physical_printer_config || p->model.objects.empty())
        return;

    PrintHostJob upload_job;

    // Snapmaker U1
    const auto preset = wxGetApp().preset_bundle->printers.get_edited_preset();
    auto       printer_config    = wxGetApp().preset_bundle->printers.get_edited_preset().config;
    auto       printer_model_opt = printer_config.option<ConfigOptionString>("printer_model");
    bool       is_snapmaker_u1   = false;
    if (printer_model_opt) {
        std::string printer_model = printer_model_opt->value;
        is_snapmaker_u1           = boost::icontains(printer_model, "Snapmaker") && boost::icontains(printer_model, "U1");
    }

    if (wxGetApp().app_config->get("use_new_connect") == "true" || is_snapmaker_u1) {
        // firstly upload and open upload download dialog,
        // get default name       
        // Obtain default output path
        fs::path default_output_file;
        try {
            // Update the background processing, so that the placeholder parser will get the correct values for the ouput file template.
            // Also if there is something wrong with the current configuration, a pop-up dialog will be shown and the export will not be performed.
            unsigned int state = this->p->update_restart_background_process(false, false);
            if (state & priv::UPDATE_BACKGROUND_PROCESS_INVALID)
                return;
            default_output_file = this->p->background_process.output_filepath_for_project("");
        } catch (const Slic3r::PlaceholderParserError& ex) {
            // Show the error with monospaced font.
            show_error(this, ex.what(), true);
            return;
        } catch (const std::exception& ex) {
            show_error(this, ex.what(), false);
            return;
        }
        default_output_file = prepare_upload_filename_for_dialog(std::move(default_output_file));

        // get file path
        auto file_path = get_partplate_list().get_curr_plate()->get_tmp_gcode_path();
        upload_job.upload_data.source_path = file_path;
        upload_job.upload_data.upload_path = default_output_file;

        // upload or print
        // Repetier specific: Query the server for the list of file groups.
        wxArrayString groups;

        // PrusaLink specific: Query the server for the list of file groups.
        wxArrayString storage_paths;
        wxArrayString storage_names;

        auto                config = get_app_config();
        PrintHostSendDialog dlg(default_output_file, PrintHostPostUploadAction::StartPrint, groups, storage_paths, storage_names,
                                config->get_bool("open_device_tab_post_upload"));
        dlg.init();
        if (dlg.ShowModal() == wxID_CANCEL) {
            return;
        }
        config->set_bool("open_device_tab_post_upload", dlg.switch_to_device_tab());
        upload_job.switch_to_device_tab    = dlg.switch_to_device_tab();
        upload_job.upload_data.upload_path = dlg.filename();
        upload_job.upload_data.post_action = dlg.post_action();
        upload_job.upload_data.group       = dlg.group();
        upload_job.upload_data.storage     = dlg.storage();


        WebPreprintDialog* dialog = new WebPreprintDialog();
        dialog->set_swtich_to_device(dlg.switch_to_device_tab());
        dialog->set_send_page(dlg.post_action() == PrintHostPostUploadAction::None);
        dialog->set_gcode_file_name(upload_job.upload_data.source_path.string());
        dialog->set_display_file_name(upload_job.upload_data.upload_path.string());
        bool res = dialog->run();

        if (dialog->is_finish()) {
            wxGetApp().mainframe->select_tab(MainFrame::TabPosition::tpMonitor);
        }

        delete dialog;

        return;
    }
    else {
        upload_job = PrintHostJob(physical_printer_config);
    }

    if (upload_job.empty())
        return;

    upload_job.upload_data.use_3mf = use_3mf;

    // Obtain default output path
    fs::path default_output_file;
    try {
        // Update the background processing, so that the placeholder parser will get the correct values for the ouput file template.
        // Also if there is something wrong with the current configuration, a pop-up dialog will be shown and the export will not be performed.
        unsigned int state = this->p->update_restart_background_process(false, false);
        if (state & priv::UPDATE_BACKGROUND_PROCESS_INVALID)
            return;
        default_output_file = this->p->background_process.output_filepath_for_project("");
    } catch (const Slic3r::PlaceholderParserError& ex) {
        // Show the error with monospaced font.
        show_error(this, ex.what(), true);
        return;
    } catch (const std::exception& ex) {
        show_error(this, ex.what(), false);
        return;
    }
    default_output_file = prepare_upload_filename_for_dialog(std::move(default_output_file));

    // Repetier specific: Query the server for the list of file groups.
    wxArrayString groups;
    {
        wxBusyCursor wait;
        upload_job.printhost->get_groups(groups);
    }

    // PrusaLink specific: Query the server for the list of file groups.
    wxArrayString storage_paths;
    wxArrayString storage_names;
    {
        wxBusyCursor wait;
        try {
            upload_job.printhost->get_storage(storage_paths, storage_names);
        } catch (const Slic3r::IOError& ex) {
            show_error(this, ex.what(), false);
            return;
        }
    }

    auto config = get_app_config();
    PrintHostSendDialog dlg(default_output_file, upload_job.printhost->get_post_upload_actions(), groups, storage_paths, storage_names, config->get_bool("open_device_tab_post_upload"));
    dlg.init();
    if (dlg.ShowModal() == wxID_OK) {
        config->set_bool("open_device_tab_post_upload", dlg.switch_to_device_tab());
        upload_job.switch_to_device_tab    = dlg.switch_to_device_tab();
        upload_job.upload_data.upload_path = dlg.filename();
        upload_job.upload_data.post_action = dlg.post_action();
        upload_job.upload_data.group       = dlg.group();
        upload_job.upload_data.storage     = dlg.storage();

        // Show "Is printer clean" dialog for PrusaConnect - Upload and print.
        if (std::string(upload_job.printhost->get_name()) == "PrusaConnect" && upload_job.upload_data.post_action == PrintHostPostUploadAction::StartPrint) {
            GUI::MessageDialog dlg(nullptr, _L("Is the printer ready? Is the print sheet in place, empty and clean?"), _L("Upload and Print"), wxOK | wxCANCEL);
            if (dlg.ShowModal() != wxID_OK)
                return;
        }

        if (use_3mf) {
            // Process gcode
            const int result = send_gcode(plate_idx, nullptr);

            if (result < 0) {
                wxString msg = _L("Abnormal print file data. Please slice again");
                show_error(this, msg, false);
                return;
            }

            upload_job.upload_data.source_path = p->m_print_job_data._3mf_path;

        }

        p->export_gcode(fs::path(), false, std::move(upload_job));
    }
}
int Plater::send_gcode(int plate_idx, Export3mfProgressFn proFn)
{
    int result = 0;
    /* generate 3mf */
    if (plate_idx == PLATE_CURRENT_IDX) {
        p->m_print_job_data.plate_idx = get_partplate_list().get_curr_plate_index();
    }
    else {
        p->m_print_job_data.plate_idx = plate_idx;
    }

    PartPlate* plate = get_partplate_list().get_curr_plate();
    try {
        p->m_print_job_data._3mf_path = fs::path(plate->get_tmp_gcode_path());
        p->m_print_job_data._3mf_path.replace_extension("3mf");
    }
    catch (std::exception&) {
        BOOST_LOG_TRIVIAL(error) << "generate 3mf path failed";
        return -1;
    }

    SaveStrategy strategy = SaveStrategy::Silence | SaveStrategy::SkipModel | SaveStrategy::WithGcode | SaveStrategy::SkipAuxiliary;
#if !BBL_RELEASE_TO_PUBLIC
    //only save model in QA environment
    std::string sel = get_app_config()->get("iot_environment");
    if (sel == ENV_PRE_HOST)
        strategy = SaveStrategy::Silence | SaveStrategy::SplitModel | SaveStrategy::WithGcode;
#endif

    result = export_3mf(p->m_print_job_data._3mf_path, strategy, plate_idx, proFn);

    return result;
}

int Plater::export_config_3mf(int plate_idx, Export3mfProgressFn proFn)
{
    int result = 0;
    /* generate 3mf */
    if (plate_idx == PLATE_CURRENT_IDX) {
        p->m_print_job_data.plate_idx = get_partplate_list().get_curr_plate_index();
    }
    else {
        p->m_print_job_data.plate_idx = plate_idx;
    }

    PartPlate* plate = get_partplate_list().get_curr_plate();
    try {
        p->m_print_job_data._3mf_config_path = fs::path(plate->get_temp_config_3mf_path());
    }
    catch (std::exception&) {
        BOOST_LOG_TRIVIAL(error) << "generate 3mf path failed";
        return -1;
    }

    SaveStrategy strategy = SaveStrategy::Silence | SaveStrategy::SkipModel | SaveStrategy::WithSliceInfo | SaveStrategy::SkipAuxiliary;
    result = export_3mf(p->m_print_job_data._3mf_config_path, strategy, plate_idx, proFn);

    return result;
}

//BBS
void Plater::send_calibration_job_finished(wxCommandEvent & evt)
{
    p->main_frame->request_select_tab(MainFrame::TabPosition::tpCalibration);
    auto calibration_panel = p->main_frame->m_calibration;
    if (calibration_panel) {
        auto curr_wizard = static_cast<CalibrationWizard*>(calibration_panel->get_tabpanel()->GetPage(evt.GetInt()));
        wxCommandEvent event(EVT_CALIBRATION_JOB_FINISHED);
        event.SetString(evt.GetString());
        event.SetEventObject(curr_wizard);
        wxPostEvent(curr_wizard, event);
    }
    evt.Skip();
}

void Plater::print_job_finished(wxCommandEvent &evt)
{
    //start print failed
    if (Slic3r::GUI::wxGetApp().get_inf_dialog_contect().empty()) {
        p->hide_select_machine_dlg();
    }
    else {
        p->enter_prepare_mode();
    }


    Slic3r::DeviceManager* dev = Slic3r::GUI::wxGetApp().getDeviceManager();
    if (!dev) return;

    dev->set_selected_machine(evt.GetString().ToStdString());
    p->main_frame->request_select_tab(MainFrame::TabPosition::tpMonitor);
    //jump to monitor and select device status panel
    MonitorPanel* curr_monitor = p->main_frame->m_monitor;
    if(curr_monitor)
       curr_monitor->get_tabpanel()->ChangeSelection(MonitorPanel::PrinterTab::PT_STATUS);
}

void Plater::send_job_finished(wxCommandEvent& evt)
{
    Slic3r::DeviceManager* dev = Slic3r::GUI::wxGetApp().getDeviceManager();
    if (!dev) return;
    //dev->set_selected_machine(evt.GetString().ToStdString());

    send_gcode_finish(evt.GetString());
    p->hide_send_to_printer_dlg();
    //p->main_frame->request_select_tab(MainFrame::TabPosition::tpMonitor);
    ////jump to monitor and select device status panel
    //MonitorPanel* curr_monitor = p->main_frame->m_monitor;
    //if (curr_monitor)
    //    curr_monitor->get_tabpanel()->ChangeSelection(MonitorPanel::PrinterTab::PT_STATUS);
}

void Plater::publish_job_finished(wxCommandEvent &evt)
{
    p->m_publish_dlg->EndModal(wxID_OK);
   // GUI::wxGetApp().load_url(evt.GetString());
   //GUI::wxGetApp().open_publish_page_dialog(evt.GetString());
}

// Called when the Eject button is pressed.
void Plater::eject_drive()
{
	wxBusyCursor wait;
    wxGetApp().removable_drive_manager()->set_and_verify_last_save_path(p->last_output_dir_path);
	wxGetApp().removable_drive_manager()->eject_drive();
}

void Plater::take_snapshot(const std::string &snapshot_name) { p->take_snapshot(snapshot_name); }
//void Plater::take_snapshot(const wxString &snapshot_name) { p->take_snapshot(snapshot_name); }
void Plater::take_snapshot(const std::string &snapshot_name, UndoRedo::SnapshotType snapshot_type) { p->take_snapshot(snapshot_name, snapshot_type); }
//void Plater::take_snapshot(const wxString &snapshot_name, UndoRedo::SnapshotType snapshot_type) { p->take_snapshot(snapshot_name, snapshot_type); }
void Plater::suppress_snapshots() { p->suppress_snapshots(); }
void Plater::allow_snapshots() { p->allow_snapshots(); }
// BBS: single snapshot
void Plater::single_snapshots_enter(SingleSnapshot *single)
{
    p->single_snapshots_enter(single);
}
void Plater::single_snapshots_leave(SingleSnapshot *single)
{
    p->single_snapshots_leave(single);
}
void Plater::undo() { p->undo(); }
void Plater::redo() { p->redo(); }
void Plater::undo_to(int selection)
{
    if (selection == 0) {
        p->undo();
        return;
    }

    const int idx = p->get_active_snapshot_index() - selection - 1;
    p->undo_redo_to(p->undo_redo_stack().snapshots()[idx].timestamp);
}
void Plater::redo_to(int selection)
{
    if (selection == 0) {
        p->redo();
        return;
    }

    const int idx = p->get_active_snapshot_index() + selection + 1;
    p->undo_redo_to(p->undo_redo_stack().snapshots()[idx].timestamp);
}
bool Plater::undo_redo_string_getter(const bool is_undo, int idx, const char** out_text)
{
    const std::vector<UndoRedo::Snapshot>& ss_stack = p->undo_redo_stack().snapshots();
    const int idx_in_ss_stack = p->get_active_snapshot_index() + (is_undo ? -(++idx) : idx);

    if (0 < idx_in_ss_stack && (size_t)idx_in_ss_stack < ss_stack.size() - 1) {
        *out_text = ss_stack[idx_in_ss_stack].name.c_str();
        return true;
    }

    return false;
}

int Plater::update_print_required_data(Slic3r::DynamicPrintConfig config, Slic3r::Model model, Slic3r::PlateDataPtrs plate_data_list, std::string file_name, std::string file_path)
{
    return p->update_print_required_data(config, model, plate_data_list, file_name, file_path);
}


void Plater::undo_redo_topmost_string_getter(const bool is_undo, std::string& out_text)
{
    const std::vector<UndoRedo::Snapshot>& ss_stack = p->undo_redo_stack().snapshots();
    const int idx_in_ss_stack = p->get_active_snapshot_index() + (is_undo ? -1 : 0);

    if (0 < idx_in_ss_stack && (size_t)idx_in_ss_stack < ss_stack.size() - 1) {
        out_text = ss_stack[idx_in_ss_stack].name;
        return;
    }

    out_text = "";
}

bool Plater::search_string_getter(int idx, const char** label, const char** tooltip)
{
    const Search::OptionsSearcher& search_list = p->sidebar->get_searcher();

    if (0 <= idx && (size_t)idx < search_list.size()) {
        search_list[idx].get_marked_label_and_tooltip(label, tooltip);
        return true;
    }

    return false;
}

void Plater::on_filaments_delete(size_t num_filaments, size_t filament_id, int replace_filament_id, const std::vector<unsigned char>& is_mixed_snapshot)
{
    // only update elements in plater
    update_filament_colors_in_full_config();

    // update fisrt print sequence and other layer sequence
    // move to partplate->on_filament_deleted
    /*Slic3r::GUI::PartPlateList &plate_list = get_partplate_list();
    for (int i = 0; i < plate_list.get_plate_count(); ++i) {
        PartPlate *part_plate = plate_list.get_plate(i);
        part_plate->update_first_layer_print_sequence_when_delete_filament(filament_id);
    }*/

    // Consume remap before updating volumes
    // This is used when merging mixed filaments to properly remap object filament IDs
    PresetBundle *preset_bundle = wxGetApp().preset_bundle;
    std::vector<unsigned int> id_remap;
    if (preset_bundle != nullptr)
        id_remap = preset_bundle->consume_last_filament_id_remap();

    // Build state map for remap if available.
    // Use the remap for both pure-delete and merge paths so that mixed
    // filaments deleted by remove_physical_filament are correctly mapped
    // to NONE instead of being shifted onto wrong IDs.
    EnforcerBlockerStateMap state_map;
    bool should_remap_states = false;
    if (!id_remap.empty()) {
        should_remap_states = true;
        if (replace_filament_id >= 0) {
            // Merge: inject the merge target into the remap so the deleted
            // physical filament maps to the target instead of 0.
            size_t old_1based = filament_id + 1;
            size_t new_1based = replace_filament_id + 1;
            if (old_1based < id_remap.size())
                id_remap[old_1based] = (unsigned int)new_1based;
        }
        for (size_t i = 0; i < state_map.size(); ++i)
            state_map[i] = EnforcerBlockerType(i);
        for (size_t i = 1; i < state_map.size(); ++i) {
            const unsigned int mapped = i < id_remap.size() ? id_remap[i] : 0;
            if (mapped == 0 || mapped >= state_map.size() || mapped > num_filaments)
                state_map[i] = EnforcerBlockerType::NONE;
            else
                state_map[i] = EnforcerBlockerType(mapped);
        }
    }

    // update mmu paint data
    // During batch physical deletion, skip per-deletion painting remap —
    // cleanup applies ONE composite remap after the loop covering all K
    // deletions.  Applying both would double-remap.
    if (p->m_batch_physical_deletion == 0) {
        for (ModelObject* mo : wxGetApp().model().objects) {
            for (ModelVolume* mv : mo->volumes) {
                if (should_remap_states) {
                    mv->remap_extruder_ids(num_filaments, state_map);
                } else {
                    mv->update_extruder_count_when_delete_filament(num_filaments, filament_id + 1,
                                                                   replace_filament_id + 1); // this function is 1 base
                }
            }
        }
    }

    // update UI
    sidebar().on_filaments_delete(filament_id);

    // update global feature filament selections
    static const char* keys[] = {"wall_filament", "sparse_infill_filament", "solid_infill_filament",
                                 "support_filament", "support_interface_filament"};
    for (auto key : keys)
        if (p->config->has(key)) {
            if (p->config->opt_int(key) == filament_id + 1)
                (*(p->config)).erase(key);
            else {
                int new_value = p->config->opt_int(key) > filament_id ? p->config->opt_int(key) - 1 : p->config->opt_int(key);
                (*(p->config)).set_key_value(key, new ConfigOptionInt(new_value));
            }
        }

    // update object/volume/support(object and volume) filament id
    sidebar().obj_list()->update_objects_list_filament_column_when_delete_filament(filament_id, num_filaments, replace_filament_id);

    // update customize gcode
    for (auto item = p->model.plates_custom_gcodes.begin(); item != p->model.plates_custom_gcodes.end(); ++item) {
        auto iter = std::remove_if(item->second.gcodes.begin(), item->second.gcodes.end(), [filament_id](const CustomGCode::Item& gcode_item) {
            return (gcode_item.type == CustomGCode::Type::ToolChange && gcode_item.extruder == filament_id + 1);
        });
        if (replace_filament_id == -1)
            item->second.gcodes.erase(iter, item->second.gcodes.end());
        else if (iter != item->second.gcodes.end()) {
            iter->extruder = replace_filament_id + 1;
        }

        for (auto& item : item->second.gcodes) {
            if (item.type == CustomGCode::Type::ToolChange && item.extruder > filament_id)
                item.extruder--;
        }
    }
}


// BBS.
void Plater::on_filaments_change(size_t num_filaments)
{
    // only update elements in plater
    update_filament_colors_in_full_config();

    const size_t old_num_filaments = sidebar().combos_filament().size();
    const bool auto_generate_before = MixedFilamentManager::auto_generate_enabled();
    const bool allow_auto_gradients = p->confirm_auto_generated_gradients(this, num_filaments);
    auto summarize_uint_vector = [](const std::vector<unsigned int> &values, size_t max_items = 24) {
        std::string out = "[";
        const size_t n = std::min(values.size(), max_items);
        for (size_t i = 0; i < n; ++i) {
            if (i > 0)
                out += ",";
            out += std::to_string(values[i]);
        }
        if (values.size() > n)
            out += ",...";
        out += "]";
        return out;
    };
    auto summarize_used_states = [](const std::vector<bool> &used, size_t max_items = 24) {
        std::string out = "[";
        size_t total = 0;
        size_t emitted = 0;
        for (size_t i = 1; i < used.size(); ++i) {
            if (!used[i])
                continue;
            ++total;
            if (emitted < max_items) {
                if (emitted > 0)
                    out += ",";
                out += std::to_string(i);
                ++emitted;
            }
        }
        if (total > emitted)
            out += ",...";
        out += "] total=" + std::to_string(total);
        return out;
    };
    PresetBundle *preset_bundle = wxGetApp().preset_bundle;
    if (preset_bundle != nullptr && auto_generate_before && !allow_auto_gradients)
        preset_bundle->update_multi_material_filament_presets(size_t(-1), old_num_filaments);
    // Consume remap before sidebar refresh, which may trigger config sync
    // paths that regenerate mixed filaments and clear this remap buffer.
    std::vector<unsigned int> id_remap;
    if (preset_bundle != nullptr)
        id_remap = preset_bundle->consume_last_filament_id_remap();

    size_t total_filaments = num_filaments;
    if (preset_bundle != nullptr)
        total_filaments = preset_bundle->mixed_filaments.total_filaments(num_filaments);

    EnforcerBlockerStateMap state_map;
    for (size_t i = 0; i < state_map.size(); ++i)
        state_map[i] = EnforcerBlockerType(i);

    bool have_explicit_remap = false;
    bool should_remap_states = false;
    if (!id_remap.empty()) {
        have_explicit_remap = true;
        should_remap_states = true;
        for (size_t i = 1; i < state_map.size(); ++i) {
            const unsigned int mapped = i < id_remap.size() ? id_remap[i] : 0;
            if (mapped == 0 || mapped >= state_map.size() || mapped > total_filaments)
                state_map[i] = EnforcerBlockerType::NONE;
            else
                state_map[i] = EnforcerBlockerType(mapped);
        }
    }

    size_t changed_entries = 0;
    std::string changed_map_preview = "[";
    for (size_t i = 1; i < state_map.size(); ++i) {
        const unsigned int mapped = unsigned(state_map[i]);
        if (mapped == i)
            continue;
        ++changed_entries;
        if (changed_entries <= 24) {
            if (changed_entries > 1)
                changed_map_preview += ",";
            changed_map_preview += std::to_string(i) + "->" + std::to_string(mapped);
        }
    }
    if (changed_entries > 24)
        changed_map_preview += ",...";
    changed_map_preview += "]";
    BOOST_LOG_TRIVIAL(warning) << "MF_REMAP on_filaments_change"
                            << " old_physical=" << old_num_filaments
                            << " new_physical=" << num_filaments
                            << " total_filaments=" << total_filaments
                            << " id_remap_size=" << id_remap.size()
                            << " id_remap=" << summarize_uint_vector(id_remap)
                            << " explicit_remap=" << (have_explicit_remap ? 1 : 0)
                            << " should_remap_states=" << (should_remap_states ? 1 : 0)
                            << " changed_entries=" << changed_entries
                            << " changed_map=" << changed_map_preview;

    size_t obj_idx = 0;
    for (ModelObject* mo : wxGetApp().model().objects) {
        size_t vol_idx = 0;
        for (ModelVolume* mv : mo->volumes) {
            std::string used_before;
            const bool has_mmu_paint = (mv != nullptr && !mv->mmu_segmentation_facets.empty());
            if (has_mmu_paint)
                used_before = summarize_used_states(mv->mmu_segmentation_facets.get_data().used_states);

            if (should_remap_states)
                mv->remap_extruder_ids(total_filaments, state_map);
            else
                mv->update_extruder_count(total_filaments);

            if (has_mmu_paint) {
                const std::string used_after = summarize_used_states(mv->mmu_segmentation_facets.get_data().used_states);
                BOOST_LOG_TRIVIAL(warning) << "MF_REMAP volume"
                                        << " obj_idx=" << obj_idx
                                        << " vol_idx=" << vol_idx
                                        << " obj_name=" << mo->name
                                        << " vol_name=" << mv->name
                                        << " before=" << used_before
                                        << " after=" << used_after;
            }
            ++vol_idx;
        }
        ++obj_idx;
    }

    // Keep UI refresh after model remap. Some UI update paths may trigger
    // scene/model sync that assumes already-remapped MMU state.
    sidebar().on_filaments_change(num_filaments);
    sidebar().obj_list()->update_objects_list_filament_column(num_filaments);

    Slic3r::GUI::PartPlateList &plate_list = get_partplate_list();
    for (int i = 0; i < plate_list.get_plate_count(); ++i) {
        PartPlate* part_plate = plate_list.get_plate(i);
        part_plate->update_first_layer_print_sequence(num_filaments);
    }
}

void Plater::on_bed_type_change(BedType bed_type)
{
    sidebar().on_bed_type_change(bed_type);
}

bool Plater::update_filament_colors_in_full_config()
{
    DynamicPrintConfig& project_config = wxGetApp().preset_bundle->project_config;
    ConfigOptionStrings* color_opt = project_config.option<ConfigOptionStrings>("filament_colour");

    p->config->option<ConfigOptionStrings>("filament_colour")->values = color_opt->values;
    return true;
}

void Plater::config_change_notification(const DynamicPrintConfig &config, const std::string& key)
{
    GLCanvas3D* view3d_canvas = get_view3D_canvas3D();
    if (key == std::string("print_sequence")) {
        auto seq_print = config.option<ConfigOptionEnum<PrintSequence>>("print_sequence");
        if (seq_print && view3d_canvas && view3d_canvas->is_initialized() && view3d_canvas->is_rendering_enabled()) {
            NotificationManager* notify_manager = get_notification_manager();
            if (seq_print->value == PrintSequence::ByObject) {
                std::string info_text = _u8L("Print By Object: \nSuggest to use auto-arrange to avoid collisions when printing.");
                notify_manager->bbl_show_seqprintinfo_notification(info_text);
            }
            else
                notify_manager->bbl_close_seqprintinfo_notification();
        }
    }
    // notification for more options
}

bool Plater::check_filament_temp_mixing(int plate_index)
{
    FilamentTempMixingDetail unused;
    return check_filament_temp_mixing(plate_index, unused);
}

bool Plater::check_filament_temp_mixing(int plate_index, FilamentTempMixingDetail& detail)
{
    detail.high_temp_slots_1based.clear();
    detail.low_temp_slots_1based.clear();

    // Boundary checks
    PartPlate* plate = nullptr;
    const DynamicPrintConfig&  full_cfg             = wxGetApp().preset_bundle->full_config();
    const ConfigOptionStrings* filament_type_option = full_cfg.option<ConfigOptionStrings>("filament_type");
    {
        if (filament_type_option == nullptr || filament_type_option->values.empty())
            return true;

        const int plate_count = p->partplate_list.get_plate_count();
        if (plate_index < 0 || plate_index >= plate_count)
            return true;

        plate = p->partplate_list.get_plate(plate_index);
        if (plate == nullptr)
            return true;

        bool has_object_on_plate = false;
        for (size_t obj_idx = 0; obj_idx < wxGetApp().model().objects.size(); ++obj_idx)
        {
            const ModelObject* model_object = wxGetApp().model().objects[obj_idx];
            if (model_object_is_on_plate(plate, obj_idx, model_object))
            {
                has_object_on_plate = true;
                break;
            }
        }
        if (!has_object_on_plate)
            return true;
    }

    // Collect filament slots actually used on this plate
    std::set<int> used_slots_0_based;
    {
        // Plate config
        const int num_filaments = static_cast<int>(filament_type_option->values.size());
        collect_filament_slots_from_config(*plate->config(), num_filaments, used_slots_0_based);

        // ModelObject config
        bool uses_default_extruder = false;
        for (size_t obj_idx = 0; obj_idx < wxGetApp().model().objects.size(); ++obj_idx)
        {
            const ModelObject* model_object = wxGetApp().model().objects[obj_idx];
            if (!model_object_is_on_plate(plate, obj_idx, model_object))
                continue;
            collect_filament_slots_from_model_config(model_object->config, num_filaments, used_slots_0_based);

            if (!model_object->config.has("extruder") || model_object->config.extruder() == 0)
                uses_default_extruder = true;

            // ModelVolume config
            for (const ModelVolume* model_volume : model_object->volumes)
            {
                collect_filament_slots_from_model_config(model_volume->config, num_filaments, used_slots_0_based);
                for (int extruder_id : model_volume->get_extruders())
                {
                    if (extruder_id >= 1 && extruder_id <= num_filaments)
                        used_slots_0_based.insert(extruder_id - 1);
                }
            }
        }

        // Collect from the Plater working config. The approach balances
        // sensitivity against false positives:
        // - Global features (wipe tower, support) always apply → always collected.
        // - Feature-specific keys (wall_filament, infill) depend on the global
        //   process defaults. They are only collected when at least one object
        //   on the plate uses the default extruder (e=0), which means those
        //   defaults WILL affect the actual slicing output.
        {
            // Always collect: features that cannot be overridden per-object.
            static const std::vector<const char*> always_collect = {"wipe_tower_filament", "support_filament", "support_interface_filament"};
            for (const char* key : always_collect)
            {
                const ConfigOptionInt* option = this->config()->option<ConfigOptionInt>(key);
                if (option != nullptr && option->value >= 1 && option->value <= num_filaments)
                    used_slots_0_based.insert(option->value - 1);
            }

            // If any object uses e=0, the global process defaults for
            // wall / infill extruders apply and must be collected.
            if (uses_default_extruder)
            {
                static const std::vector<const char*> default_keys = {"wall_filament", "sparse_infill_filament", "solid_infill_filament"};
                for (const char* key : default_keys)
                {
                    const ConfigOptionInt* option = config()->option<ConfigOptionInt>(key);
                    if (option != nullptr && option->value >= 1 && option->value <= num_filaments)
                        used_slots_0_based.insert(option->value - 1);
                }
            }
        }

        // Resolve the global default extruder if any object on this plate
        // uses extruder=0. p->config does not include the "extruder" key
        // (it is not in the initializer list at priv constructor), so we
        // must read it from full_config() instead.
        if (uses_default_extruder)
        {
            const ConfigOptionInt* extruder_opt = full_cfg.option<ConfigOptionInt>("extruder");
            if (extruder_opt != nullptr && extruder_opt->value >= 1 && extruder_opt->value <= num_filaments)
                used_slots_0_based.insert(extruder_opt->value - 1);
        }
    }
    if (used_slots_0_based.empty())
        return true;

    // Read filament_is_high_temperature directly from each filament preset's
    // own config rather than through full_config(). full_config() builds a
    // merged snapshot that may lag behind when called from Sidebar hooks
    // (the edited preset config hasn't been committed yet).
    //
    // `used_slots_0_based` is a std::set<int> so iteration is ascending and
    // de-duplicated; the 1-based vectors we fill here inherit that ordering.
    bool has_high = false, has_low = false;
    {
        PresetBundle* bundle = wxGetApp().preset_bundle;
        for (int slot : used_slots_0_based)
        {
            if (slot < 0 || slot >= static_cast<int>(bundle->filament_presets.size()))
                continue;
            const Preset* preset = bundle->filaments.find_preset(bundle->filament_presets[slot], true);
            if (preset == nullptr)
                continue;
            const bool is_high = preset->config.opt_bool("filament_is_high_temperature", 0);
            if (is_high)
            {
                has_high = true;
                detail.high_temp_slots_1based.push_back(slot + 1);
            }
            else
            {
                has_low = true;
                detail.low_temp_slots_1based.push_back(slot + 1);
            }
        }
    }

    const bool compatible = !(has_high && has_low);
    if (compatible)
    {
        // No conflict — clear detail so callers can't read stale partial data.
        detail.high_temp_slots_1based.clear();
        detail.low_temp_slots_1based.clear();
    }
    return compatible;
}

bool Plater::check_filament_temp_mixing()
{
    return check_filament_temp_mixing(p->partplate_list.get_curr_plate_index());
}

Plater::FilamentTempMixingState Plater::get_filament_temp_mixing_state(int plate_index)
{
    if (check_filament_temp_mixing(plate_index))
        return FilamentTempMixingState::Compatible;

    return wxGetApp().app_config->get_bool("allow_filament_temp_mixing") ?
        FilamentTempMixingState::AllowedWarning :
        FilamentTempMixingState::BlockedError;
}

Plater::FilamentTempMixingState Plater::get_filament_temp_mixing_state()
{
    return get_filament_temp_mixing_state(p->partplate_list.get_curr_plate_index());
}

bool Plater::is_plate_blocked_by_filament_temp_mixing(int plate_index)
{
    return get_filament_temp_mixing_state(plate_index) == FilamentTempMixingState::BlockedError;
}

bool Plater::is_plate_blocked_by_filament_temp_mixing_cached(int plate_index) const
{
    if (plate_index < 0 || plate_index >= static_cast<int>(p->filament_temp_mixing_blocked_cache.size()))
        return false;
    return p->filament_temp_mixing_blocked_cache[plate_index];
}

bool Plater::has_sliceable_plate_for_slice_all()
{
    return find_next_sliceable_plate_for_slice_all(0) >= 0;
}

bool Plater::is_plate_sliceable(int plate_index)
{
    PartPlate* plate = p->partplate_list.get_plate(plate_index);
    if (plate == nullptr || !plate->can_slice())
        return false;
    // GUI-layer blockers live here so call sites cannot drift. Add new
    // blockers as additional early-returns in this function; do NOT branch
    // on them at individual GLCanvas3D / MainFrame call sites.
    //
    // Mixing uses the cached lookup (refreshed in sync_filament_temp_mixing_notification,
    // which runs at every state-changing event) because the rendering hot path in
    // GLCanvas3D::_render_imgui_select_plate_toolbar hits this for every plate every
    // frame. cold_plate / flow_ratio_zero have no cached form yet; they recompute on
    // each call.
    if (is_plate_blocked_by_filament_temp_mixing_cached(plate_index))
        return false;
    if (is_plate_blocked_by_cold_plate(plate_index))
        return false;
    if (is_plate_blocked_by_flow_ratio_zero(plate_index))
        return false;
    return true;
}

int Plater::find_next_sliceable_plate_for_slice_all(int start_plate_index)
{
    const int plate_count = p->partplate_list.get_plate_count();
    if (start_plate_index < 0)
        start_plate_index = 0;

    for (int plate_index = start_plate_index; plate_index < plate_count; ++plate_index)
    {
        if (is_plate_sliceable(plate_index))
            return plate_index;
    }

    return -1;
}

bool Plater::sync_filament_temp_mixing_notification()
{
    // 1. Always close the previously-pushed notifications using their exact
    //    cached text. NotificationManager::close_validate_* matches on text,
    //    so we cannot use a freshly regenerated template string here — it
    //    would not match the previous (possibly different) body.
    //    This runs before the curr_plate null check so that an early return
    //    does not leak a stale notification on the screen.
    NotificationManager* nm = get_notification_manager();
    if (!p->filament_temp_mixing_last_error_text.empty())
    {
        nm->close_validate_error_notification(p->filament_temp_mixing_last_error_text);
        p->filament_temp_mixing_last_error_text.clear();
    }
    if (!p->filament_temp_mixing_last_warning_text.empty())
    {
        nm->close_validate_warning_notification(p->filament_temp_mixing_last_warning_text);
        p->filament_temp_mixing_last_warning_text.clear();
    }

    // 2. Refresh the per-plate blocked cache used by hot rendering paths.
    //    sync_filament_temp_mixing_notification() is invoked at every state-
    //    changing event (plate switch, filament change, bed-type change, etc.),
    //    so refreshing here keeps the cache fresh without recomputing every
    //    frame in GLCanvas3D::_render_imgui_select_plate_toolbar.
    const int plate_count = p->partplate_list.get_plate_count();
    p->filament_temp_mixing_blocked_cache.assign(plate_count, false);
    for (int i = 0; i < plate_count; ++i)
    {
        p->filament_temp_mixing_blocked_cache[i] =
            (get_filament_temp_mixing_state(i) == FilamentTempMixingState::BlockedError);
    }

    PartPlate* curr_plate = get_partplate_list().get_curr_plate();
    if (curr_plate == nullptr)
    {
        BOOST_LOG_TRIVIAL(warning) << "[Plater] sync_filament_temp_mixing_notification: curr_plate is null";
        return true;
    }

    const int curr_plate_index = get_partplate_list().get_curr_plate_index();
    const FilamentTempMixingState mixing_state = get_filament_temp_mixing_state(curr_plate_index);
    bool slicing_allowed = true;

    // 3. Compute the per-plate detail (used by both warning and error text).
    FilamentTempMixingDetail detail;
    if (mixing_state != FilamentTempMixingState::Compatible)
        check_filament_temp_mixing(curr_plate_index, detail);

    switch (mixing_state)
    {
    case FilamentTempMixingState::Compatible:
        // Filament temp mixing is compatible — only clear our own notification,
        // do NOT touch m_apply_invalid. Bed type mismatch or other validation
        // errors must not be cleared by the filament temp mixing system.
        slicing_allowed = true;
        break;
    case FilamentTempMixingState::AllowedWarning: {
        // push_notification stores the body verbatim (no auto prefix), while
        // close_validate_warning_notification(text) compares against
        // "WARNING:\n" + text. So the pushed body must include the WARNING
        // prefix, but the cached value must NOT — that way close-time text
        // matches the pushed text exactly.
        const std::string warning_body = filament_temp_mixing_warning_text(detail);
        nm->push_notification(
            NotificationType::ValidateWarning,
            NotificationManager::NotificationLevel::WarningNotificationLevel,
            tr_u8("WARNING:") + "\n" + warning_body);
        p->filament_temp_mixing_last_warning_text = warning_body;
        slicing_allowed = true;
        break;
    }
    case FilamentTempMixingState::BlockedError: {
        StringObjectException err;
        err.type   = STRING_EXCEPT_FILAMENTS_MIXING_TEMP;
        err.string = filament_temp_mixing_error_text(detail);
        nm->push_validate_error_notification(err);
        p->filament_temp_mixing_last_error_text = err.string;
        // Blocking is enforced through get_enable_slice_status() / find_next_sliceable_plate_for_slice_all()
        // which independently check is_plate_blocked_by_filament_temp_mixing().
        // Do NOT set m_apply_invalid - that flag belongs to the background validation system.
        slicing_allowed = false;
        break;
    }
    default:
        BOOST_LOG_TRIVIAL(warning) << "[Plater] sync_filament_temp_mixing_notification: unknown mixing_state " << static_cast<int>(mixing_state);
        slicing_allowed = true;
        break;
    }

    p->filament_temp_mixing_notification_initialized = true;
    p->filament_temp_mixing_notification_plate = curr_plate_index;
    p->filament_temp_mixing_notification_state = mixing_state;

    const bool can_slice = curr_plate->can_slice() && slicing_allowed;
    p->main_frame->update_slice_print_status(MainFrame::eEventPlateUpdate, can_slice);
    return slicing_allowed;
}

bool Plater::check_flow_ratio_zero(int plate_index, FlowRatioZeroDetail& detail)
{
    detail.offender_slots_1based.clear();

    // Boundary checks (mirror check_filament_temp_mixing).
    PartPlate* plate = nullptr;
    const DynamicPrintConfig&  full_cfg             = wxGetApp().preset_bundle->full_config();
    const ConfigOptionStrings* filament_type_option = full_cfg.option<ConfigOptionStrings>("filament_type");
    {
        if (filament_type_option == nullptr || filament_type_option->values.empty())
            return true;

        const int plate_count = p->partplate_list.get_plate_count();
        if (plate_index < 0 || plate_index >= plate_count)
            return true;

        plate = p->partplate_list.get_plate(plate_index);
        if (plate == nullptr)
            return true;

        bool has_object_on_plate = false;
        for (size_t obj_idx = 0; obj_idx < wxGetApp().model().objects.size(); ++obj_idx) {
            const ModelObject* model_object = wxGetApp().model().objects[obj_idx];
            if (model_object_is_on_plate(plate, obj_idx, model_object)) {
                has_object_on_plate = true;
                break;
            }
        }
        if (!has_object_on_plate)
            return true;
    }

    // Collect filament slots actually used on this plate (same shared logic).
    std::set<int> used_slots_0_based;
    {
        const int num_filaments = static_cast<int>(filament_type_option->values.size());
        collect_filament_slots_from_config(*plate->config(), num_filaments, used_slots_0_based);

        bool uses_default_extruder = false;
        for (size_t obj_idx = 0; obj_idx < wxGetApp().model().objects.size(); ++obj_idx) {
            const ModelObject* model_object = wxGetApp().model().objects[obj_idx];
            if (!model_object_is_on_plate(plate, obj_idx, model_object))
                continue;
            collect_filament_slots_from_model_config(model_object->config, num_filaments, used_slots_0_based);

            if (!model_object->config.has("extruder") || model_object->config.extruder() == 0)
                uses_default_extruder = true;

            for (const ModelVolume* model_volume : model_object->volumes) {
                collect_filament_slots_from_model_config(model_volume->config, num_filaments, used_slots_0_based);
                for (int extruder_id : model_volume->get_extruders()) {
                    if (extruder_id >= 1 && extruder_id <= num_filaments)
                        used_slots_0_based.insert(extruder_id - 1);
                }
            }
        }

        {
            static const std::vector<const char*> always_collect = {"wipe_tower_filament", "support_filament", "support_interface_filament"};
            for (const char* key : always_collect) {
                const ConfigOptionInt* option = this->config()->option<ConfigOptionInt>(key);
                if (option != nullptr && option->value >= 1 && option->value <= num_filaments)
                    used_slots_0_based.insert(option->value - 1);
            }

            if (uses_default_extruder) {
                static const std::vector<const char*> default_keys = {"wall_filament", "sparse_infill_filament", "solid_infill_filament"};
                for (const char* key : default_keys) {
                    const ConfigOptionInt* option = config()->option<ConfigOptionInt>(key);
                    if (option != nullptr && option->value >= 1 && option->value <= num_filaments)
                        used_slots_0_based.insert(option->value - 1);
                }
            }
        }

        if (uses_default_extruder) {
            const ConfigOptionInt* extruder_opt = full_cfg.option<ConfigOptionInt>("extruder");
            if (extruder_opt != nullptr && extruder_opt->value >= 1 && extruder_opt->value <= num_filaments)
                used_slots_0_based.insert(extruder_opt->value - 1);
        }
    }
    if (used_slots_0_based.empty())
        return true;

    // Read filament_flow_ratio from each used preset's own config (same defensive
    // pattern as filament_is_high_temperature in check_filament_temp_mixing).
    // `<=` rather than `== 0.0` to dodge floating-point comparison traps.
    {
        PresetBundle* bundle = wxGetApp().preset_bundle;
        for (int slot : used_slots_0_based) {
            if (slot < 0 || slot >= static_cast<int>(bundle->filament_presets.size()))
                continue;
            const Preset* preset = bundle->filaments.find_preset(bundle->filament_presets[slot], true);
            if (preset == nullptr)
                continue;
            const double ratio = preset->config.opt_float("filament_flow_ratio", 0);
            if (ratio <= 0.0)
                detail.offender_slots_1based.push_back(slot + 1);
        }
    }

    return detail.offender_slots_1based.empty();
}

bool Plater::is_plate_blocked_by_flow_ratio_zero(int plate_index)
{
    FlowRatioZeroDetail unused;
    return !check_flow_ratio_zero(plate_index, unused);
}

bool Plater::sync_flow_ratio_zero_notification()
{
    PartPlate* curr_plate = get_partplate_list().get_curr_plate();
    if (curr_plate == nullptr) {
        BOOST_LOG_TRIVIAL(warning) << "[Plater] sync_flow_ratio_zero_notification: curr_plate is null";
        return true;
    }

    const int curr_plate_index = get_partplate_list().get_curr_plate_index();
    FlowRatioZeroDetail detail;
    const bool blocked = !check_flow_ratio_zero(curr_plate_index, detail);
    bool slicing_allowed = true;

    NotificationManager* nm = get_notification_manager();

    // 1. Close any previously-pushed banner using its exact cached text.
    //    close_validate_error_notification() matches by text, so we must use
    //    the cached string (which may differ from a freshly regenerated body).
    if (!p->flow_ratio_zero_last_error_text.empty()) {
        nm->close_validate_error_notification(p->flow_ratio_zero_last_error_text);
        p->flow_ratio_zero_last_error_text.clear();
    }

    // 2. Push a fresh banner only if blocked. Blocking itself is enforced
    //    independently by get_enable_slice_status() / find_next_sliceable_plate_for_slice_all()
    //    via is_plate_blocked_by_flow_ratio_zero(), so we do NOT touch m_apply_invalid
    //    (that flag belongs to the background validation system).
    if (blocked) {
        StringObjectException err;
        err.type   = STRING_EXCEPT_FLOW_RATIO_ZERO;
        err.string = flow_ratio_zero_error_text(detail);
        nm->push_validate_error_notification(err);
        p->flow_ratio_zero_last_error_text = err.string;
        slicing_allowed = false;
    }

    const bool can_slice = curr_plate->can_slice() && slicing_allowed;
    p->main_frame->update_slice_print_status(MainFrame::eEventPlateUpdate, can_slice);
    return slicing_allowed;
}

Plater::ColdPlateCompatResult Plater::get_cold_plate_compat_state(int plate_index) const
{
    ColdPlateCompatResult result;

    // Boundary check
    const int plate_count = p->partplate_list.get_plate_count();
    if (plate_index < 0 || plate_index >= plate_count) {
        BOOST_LOG_TRIVIAL(warning) << "[Plater] get_cold_plate_compat_state: invalid plate_index=" << plate_index;
        return result;
    }

    // Early exit: only relevant under the Cool Steel Plate bed type.
    // Cool Steel Plate reuses the Supertack bed-type enumerator (its config key
    // is "Supertack Plate" in s_keys_map_BedType), so compare against btSuperTack directly.
    const BedType curr_bed = wxGetApp().preset_bundle->project_config.opt_enum<BedType>("curr_bed_type");
    if (curr_bed != btSuperTack)
        return result;

    // Need a non-null plate and at least one object on it
    const DynamicPrintConfig& full_cfg = wxGetApp().preset_bundle->full_config();
    const ConfigOptionStrings* filament_type_option = full_cfg.option<ConfigOptionStrings>("filament_type");
    if (filament_type_option == nullptr || filament_type_option->values.empty())
        return result;

    PartPlate* plate = p->partplate_list.get_plate(plate_index);
    if (plate == nullptr)
        return result;

    bool has_object_on_plate = false;
    for (size_t obj_idx = 0; obj_idx < wxGetApp().model().objects.size(); ++obj_idx) {
        const ModelObject* model_object = wxGetApp().model().objects[obj_idx];
        if (model_object_is_on_plate(plate, obj_idx, model_object)) {
            has_object_on_plate = true;
            break;
        }
    }
    if (!has_object_on_plate)
        return result;

    // Collect used slots once (shared with filament_temp_mixing for parity).
    std::set<int> used_slots_0_based;
    const int num_filaments = static_cast<int>(filament_type_option->values.size());
    collect_used_filament_slots_on_plate(plate, num_filaments, this->config(), full_cfg, used_slots_0_based);
    if (used_slots_0_based.empty())
        return result;

    // Single pass over used slots: classify each preset into unsupported /
    // TPU. State is derived at the end: BlockedError wins over SeriousWarning.
    PresetBundle* bundle = wxGetApp().preset_bundle;
    for (int slot : used_slots_0_based) {
        if (slot < 0 || slot >= static_cast<int>(bundle->filament_presets.size()))
            continue;
        const Preset* preset = bundle->filaments.find_preset(bundle->filament_presets[slot], true);
        if (preset == nullptr)
            continue;

        const ConfigOptionStrings* ftype = preset->config.option<ConfigOptionStrings>("filament_type");
        const std::string type_str = (ftype != nullptr && !ftype->values.empty())
            ? ftype->values[0]
            : std::string("?");
        if (type_str == "TPU") {
            result.uses_tpu = true;
            result.tpu_slots_1_based.push_back(slot + 1);
        }

        const int t_other = preset->config.opt_int("supertack_plate_temp", 0);
        const int t_first = preset->config.opt_int("supertack_plate_temp_initial_layer", 0);
        if (t_first <= 0 || t_other <= 0)
            result.unsupported_slots_1_based.push_back(slot + 1);
    }

    if (!result.unsupported_slots_1_based.empty())
        result.state = ColdPlateCompatState::BlockedError;
    else if (result.uses_tpu)
        result.state = ColdPlateCompatState::SeriousWarning;
    else
        result.state = ColdPlateCompatState::Compatible;
    return result;
}

bool Plater::is_plate_blocked_by_cold_plate(int plate_index) const
{
    return get_cold_plate_compat_state(plate_index).state == ColdPlateCompatState::BlockedError;
}

bool Plater::sync_cold_plate_notification()
{
    PartPlate* curr_plate = get_partplate_list().get_curr_plate();
    if (curr_plate == nullptr) {
        BOOST_LOG_TRIVIAL(warning) << "[Plater] sync_cold_plate_notification: curr_plate is null";
        return true;
    }

    const int                   curr_plate_index = get_partplate_list().get_curr_plate_index();
    const ColdPlateCompatResult compat          = get_cold_plate_compat_state(curr_plate_index);
    const ColdPlateCompatState  state           = compat.state;
    bool                        slicing_allowed = true;

    // Always close the previously-pushed notification first. close_validate_error_notification
    // and close_slicing_serious_warning_notification use exact-text matching, so we MUST pass
    // the previously-pushed text (cached in p->cold_plate_last_*), not a fresh empty/template
    // text. This is the only way to guarantee stale notifications disappear across state changes.
    if (!p->cold_plate_last_error_text.empty()) {
        get_notification_manager()->close_validate_error_notification(p->cold_plate_last_error_text);
        p->cold_plate_last_error_text.clear();
    }
    if (!p->cold_plate_last_serious_warning_text.empty()) {
        get_notification_manager()->close_slicing_serious_warning_notification(p->cold_plate_last_serious_warning_text);
        p->cold_plate_last_serious_warning_text.clear();
    }

    switch (state)
    {
    case ColdPlateCompatState::Compatible:
        slicing_allowed = true;
        break;
    case ColdPlateCompatState::SeriousWarning: {
        // TPU is compatible but warrants a non-blocking serious warning.
        const std::string text = cold_plate_serious_warning_text(compat.tpu_slots_1_based);
        get_notification_manager()->push_slicing_serious_warning_notification(text, std::vector<ModelObject const*>());
        p->cold_plate_last_serious_warning_text = text;
        slicing_allowed = true;
        break;
    }
    case ColdPlateCompatState::BlockedError: {
        // Merge all unsupported filaments (already collected in compat) into a single error notification.
        StringObjectException err;
        err.type   = STRING_EXCEPT_COLD_PLATE_INCOMPATIBLE;
        err.string = cold_plate_error_text(compat.unsupported_slots_1_based);
        get_notification_manager()->push_validate_error_notification(err);
        p->cold_plate_last_error_text = err.string;
        slicing_allowed = false;
        break;
    }
    default:
        BOOST_LOG_TRIVIAL(warning) << "[Plater] sync_cold_plate_notification: unknown state "
                                   << static_cast<int>(state);
        slicing_allowed = true;
        break;
    }

    p->cold_plate_notification_initialized = true;
    p->cold_plate_notification_plate       = curr_plate_index;
    p->cold_plate_notification_state       = state;

    // Button enable/disable is gated by MainFrame::get_enable_slice_status(), which checks
    // is_plate_blocked_by_cold_plate() directly. The can_slice arg is ignored for
    // eEventPlateUpdate — pass true to match filament_temp_mixing's contract.
    const bool can_slice = curr_plate->can_slice() && slicing_allowed;
    p->main_frame->update_slice_print_status(MainFrame::eEventPlateUpdate, can_slice);
    return slicing_allowed;
}

bool Plater::guard_before_slice_plate()
{
    sync_filament_temp_mixing_notification();
    sync_flow_ratio_zero_notification();
    sync_cold_plate_notification();
    return confirm_filament_temp_mixing_before_slice();
}

bool Plater::guard_before_slice_all()
{
    sync_flow_ratio_zero_notification();
    return confirm_filament_temp_mixing_before_slice_all();
}

bool Plater::confirm_filament_temp_mixing_before_slice()
{
    const FilamentTempMixingState state = get_filament_temp_mixing_state();
    switch (state)
    {
    case FilamentTempMixingState::Compatible:
        return true;
    case FilamentTempMixingState::BlockedError:
        sync_filament_temp_mixing_notification();
        sync_cold_plate_notification();
        return false;
    case FilamentTempMixingState::AllowedWarning:
        break;
    default:
        BOOST_LOG_TRIVIAL(warning) << "[Plater] confirm_filament_temp_mixing_before_slice: unknown state "
                                   << static_cast<int>(state);
        return true;
    }

    // Show the actual high/low filament breakdown so the user can see which
    // slots are involved before deciding. MessageDialog takes wxString, so
    // convert the UTF-8 body produced by the text builder.
    FilamentTempMixingDetail detail;
    check_filament_temp_mixing(p->partplate_list.get_curr_plate_index(), detail);
    const std::string body = filament_temp_mixing_warning_text(detail)
                             + "\n"
                             + tr_u8("Do you want to continue?");

    MessageDialog dlg(this, wxString::FromUTF8(body.c_str()),
                      _L("Confirm slicing"), wxICON_WARNING | wxOK | wxCANCEL);
    dlg.SetButtonLabel(wxID_OK, _L("Confirm"));
    dlg.SetButtonLabel(wxID_CANCEL, _L("Cancel"));
    return dlg.ShowModal() == wxID_OK;
}

bool Plater::confirm_filament_temp_mixing_before_slice_all()
{
    if (!has_sliceable_plate_for_slice_all())
        return false;

    // Collect every sliceable, not-yet-sliced plate that is currently in the
    // AllowedWarning state, along with its high/low filament breakdown so the
    // confirmation dialog can show per-plate details.
    std::vector<PlateMixingInfo> mixing_plates;
    for (int plate_index = 0; plate_index < p->partplate_list.get_plate_count(); ++plate_index)
    {
        PartPlate* plate = p->partplate_list.get_plate(plate_index);
        if (plate == nullptr || !plate->can_slice() || plate->is_slice_result_valid())
            continue;
        if (get_filament_temp_mixing_state(plate_index) != FilamentTempMixingState::AllowedWarning)
            continue;

        PlateMixingInfo info;
        info.plate_index_1based = plate_index + 1;
        // state == AllowedWarning guarantees a mixing conflict exists, so the
        // detail will be populated.
        check_filament_temp_mixing(plate_index, info.detail);
        mixing_plates.push_back(info);
    }

    if (mixing_plates.empty())
        return true;

    const std::string body = filament_temp_mixing_warning_text_slice_all(mixing_plates)
                             + "\n"
                             + tr_u8("Do you want to continue?");

    MessageDialog dlg(this, wxString::FromUTF8(body.c_str()),
                      _L("Confirm slicing"), wxICON_WARNING | wxOK | wxCANCEL);
    dlg.SetButtonLabel(wxID_OK, _L("Confirm"));
    dlg.SetButtonLabel(wxID_CANCEL, _L("Cancel"));
    return dlg.ShowModal() == wxID_OK;
}

void Plater::notify_filament_usage_changed()
{
    if (p->filament_usage_sync_pending.exchange(true))
        return;

    wxQueueEvent(this, new SimpleEvent(EVT_FILAMENT_USAGE_CHANGED, this));
}

void Plater::on_config_change(const DynamicPrintConfig &config)
{
    bool update_scheduled = false;
    bool bed_shape_changed = false;
    //bool print_sequence_changed = false;
    t_config_option_keys diff_keys = p->config->diff(config);
    for (auto opt_key : diff_keys) {
        if (opt_key == "filament_colour") {
            update_scheduled = true; // update should be scheduled (for update 3DScene) #2738

            if (update_filament_colors_in_full_config()) {
                p->sidebar->obj_list()->update_filament_colors();
                p->sidebar->update_dynamic_filament_list();
                continue;
            }
        }
        if (opt_key == "material_colour") {
            update_scheduled = true; // update should be scheduled (for update 3DScene)
        }

        p->config->set_key_value(opt_key, config.option(opt_key)->clone());
        if (opt_key == "printer_technology") {
            this->set_printer_technology(config.opt_enum<PrinterTechnology>(opt_key));
            // print technology is changed, so we should to update a search list
            p->sidebar->update_searcher();
            p->reset_gcode_toolpaths();
            p->view3D->get_canvas3d()->reset_sequential_print_clearance();
            //BBS: invalid all the slice results
            p->partplate_list.invalid_all_slice_result();
        }
        //BBS: add bed_exclude_area
        else if (opt_key == "printable_area" || opt_key == "bed_exclude_area"
            || opt_key == "bed_custom_texture" || opt_key == "bed_custom_model"
            || opt_key == "extruder_clearance_height_to_lid"
            || opt_key == "extruder_clearance_height_to_rod") {
            bed_shape_changed = true;
            update_scheduled = true;
        }
        else if (opt_key == "bed_shape" || opt_key == "bed_custom_texture" || opt_key == "bed_custom_model") {
            bed_shape_changed = true;
            update_scheduled = true;
        }
        else if (boost::starts_with(opt_key, "enable_prime_tower") ||
            boost::starts_with(opt_key, "prime_tower") ||
            boost::starts_with(opt_key, "wipe_tower") ||
            opt_key == "filament_minimal_purge_on_wipe_tower" ||
            opt_key == "single_extruder_multi_material" ||
            // BBS
            opt_key == "prime_volume") {
            update_scheduled = true;
        }
        else if(opt_key == "extruder_colour") {
            update_scheduled = true;
            //p->sidebar->obj_list()->update_extruder_colors();
        }
        else if (opt_key == "printable_height") {
            bed_shape_changed = true;
            update_scheduled = true;
        }
        else if (opt_key == "print_sequence") {
            update_scheduled = true;
            //print_sequence_changed = true;
        }
        else if (opt_key == "printer_model") {
            p->reset_gcode_toolpaths();
            // update to force bed selection(for texturing)
            bed_shape_changed = true;
            update_scheduled = true;
        }
        // Orca: update when *_filament changed
        else if (opt_key == "support_interface_filament" || opt_key == "support_filament" || opt_key == "wall_filament" ||
                 opt_key == "sparse_infill_filament" || opt_key == "solid_infill_filament") {
            update_scheduled = true;
        }
    }

    if (bed_shape_changed)
        set_bed_shape();

    config_change_notification(config, std::string("print_sequence"));

    if (update_scheduled)
        update();

    if (p->main_frame->is_loaded()) {
        this->p->schedule_background_process();
        update_title_dirty_status();
    }

    notify_filament_usage_changed();
}

void Plater::set_bed_shape() const
{
    std::string texture_filename;
    auto bundle = wxGetApp().preset_bundle;
    if (bundle != nullptr) {
        const Preset* curr = &bundle->printers.get_selected_preset();
        if (curr->is_system)
            texture_filename = PresetUtils::system_printer_bed_texture(*curr);
        else {
            auto *printer_model = curr->config.opt<ConfigOptionString>("printer_model");
            if (printer_model != nullptr && ! printer_model->value.empty()) {
                texture_filename = bundle->get_texture_for_printer_model(printer_model->value);
            }
        }
    }
    set_bed_shape(p->config->option<ConfigOptionPoints>("printable_area")->values,
        //BBS: add bed exclude areas
        p->config->option<ConfigOptionPoints>("bed_exclude_area")->values,
        p->config->option<ConfigOptionFloat>("printable_height")->value,
        p->config->option<ConfigOptionString>("bed_custom_texture")->value.empty() ? texture_filename : p->config->option<ConfigOptionString>("bed_custom_texture")->value,
        p->config->option<ConfigOptionString>("bed_custom_model")->value);
}

//BBS: add bed exclude area
void Plater::set_bed_shape(const Pointfs& shape, const Pointfs& exclude_area, const double printable_height, const std::string& custom_texture, const std::string& custom_model, bool force_as_custom) const
{
    p->set_bed_shape(make_counter_clockwise(shape), exclude_area, printable_height, custom_texture, custom_model, force_as_custom);
}

void Plater::force_filament_colors_update()
{
//BBS: filament_color logic has been moved out of filament setting
#if 0
    bool update_scheduled = false;
    DynamicPrintConfig* config = p->config;
    const std::vector<std::string> filament_presets = wxGetApp().preset_bundle->filament_presets;
    if (filament_presets.size() > 1 &&
        p->config->option<ConfigOptionStrings>("filament_colour")->values.size() == filament_presets.size())
    {
        const PresetCollection& filaments = wxGetApp().preset_bundle->filaments;
        std::vector<std::string> filament_colors;
        filament_colors.reserve(filament_presets.size());

        for (const std::string& filament_preset : filament_presets)
            filament_colors.push_back(filaments.find_preset(filament_preset, true)->config.opt_string("filament_colour", (unsigned)0));

        if (config->option<ConfigOptionStrings>("filament_colour")->values != filament_colors) {
            config->option<ConfigOptionStrings>("filament_colour")->values = filament_colors;
            update_scheduled = true;
        }
    }

    if (update_scheduled) {
        update();
        p->sidebar->obj_list()->update_filament_colors();
    }

    if (p->main_frame->is_loaded())
        this->p->schedule_background_process();
#endif
}

void Plater::force_print_bed_update()
{
    // Fill in the printer model key with something which cannot possibly be valid, so that Plater::on_config_change() will update the print bed
    // once a new Printer profile config is loaded.
    p->config->opt_string("printer_model", true) = "bbl_empty";
}

void Plater::on_activate()
{
    this->p->show_delayed_error_message();
}

// Get vector of extruder colors considering filament color, if extruder color is undefined.
std::vector<std::string> Plater::get_extruder_colors_from_plater_config(const GCodeProcessorResult* const result, bool include_mixed) const
{
    if (wxGetApp().is_gcode_viewer() && result != nullptr)
        return result->extruder_colors;
    else {
        if (wxGetApp().preset_bundle == nullptr)
            return {};

        const Slic3r::DynamicPrintConfig* config = &wxGetApp().preset_bundle->project_config;
        std::vector<std::string> filament_colors;
        if (!config->has("filament_colour")) // in case of a SLA print
            return filament_colors;

        filament_colors = (config->option<ConfigOptionStrings>("filament_colour"))->values;
        const size_t num_physical = static_cast<size_t>(std::max(wxGetApp().filaments_cnt(), 0));
        filament_colors.resize(num_physical, "#26A69A");

        if (include_mixed) {
            // Append display colours for enabled mixed (virtual) filaments.
            const auto &mixed_mgr = wxGetApp().preset_bundle->mixed_filaments;
            for (const auto &dc : mixed_mgr.display_colors())
                filament_colors.push_back(dc);
        }

        return filament_colors;
    }
}

/* Get vector of colors used for rendering of a Preview scene in "Color print" mode
 * It consists of extruder colors and colors, saved in model.custom_gcode_per_print_z
 */
std::vector<std::string> Plater::get_colors_for_color_print(const GCodeProcessorResult* const result) const
{
    std::vector<std::string> colors = get_extruder_colors_from_plater_config(result);

    if (wxGetApp().is_gcode_viewer() && result != nullptr) {
        for (const CustomGCode::Item& code : result->custom_gcode_per_print_z) {
            if (code.type == CustomGCode::ColorChange)
                colors.emplace_back(code.color);
        }
    }
    else {
        //BBS
        colors.reserve(colors.size() + p->model.get_curr_plate_custom_gcodes().gcodes.size());
        for (const CustomGCode::Item& code : p->model.get_curr_plate_custom_gcodes().gcodes) {
            if (code.type == CustomGCode::ColorChange)
                colors.emplace_back(code.color);
        }
    }

    return colors;
}

void Plater::set_global_filament_map(const std::vector<int>& filament_map)
{
    auto& project_config                                            = wxGetApp().preset_bundle->project_config;
    project_config.option<ConfigOptionInts>("filament_map")->values = filament_map;
}

std::vector<int> Plater::get_global_filament_map() const
{
    auto& project_config = wxGetApp().preset_bundle->project_config;
    return project_config.option<ConfigOptionInts>("filament_map")->values;
}

wxWindow* Plater::get_select_machine_dialog()
{
    return p->m_select_machine_dlg;
}

void Plater::update_print_error_info(int code, std::string msg, std::string extra)
{
    if (p->m_select_machine_dlg) {
        p->m_select_machine_dlg->update_print_error_info(code, msg, extra);
    }

    if (p->m_send_to_sdcard_dlg) {
        p->m_send_to_sdcard_dlg->update_print_error_info(code, msg, extra);
    }
    if (p->main_frame->m_calibration)
        p->main_frame->m_calibration->update_print_error_info(code, msg, extra);
}

wxString Plater::get_project_filename(const wxString& extension) const
{
    return p->get_project_filename(extension);
}

wxString Plater::get_export_gcode_filename(const wxString & extension, bool only_filename, bool export_all) const
{
    return p->get_export_gcode_filename(extension, only_filename, export_all);
}

void Plater::set_project_filename(const wxString& filename)
{
    p->set_project_filename(filename);
}

bool Plater::is_export_gcode_scheduled() const
{
    return p->background_process.is_export_scheduled();
}

const Selection &Plater::get_selection() const
{
    return p->get_selection();
}

int Plater::get_selected_object_idx()
{
    return p->get_selected_object_idx();
}

bool Plater::is_single_full_object_selection() const
{
    return p->get_selection().is_single_full_object();
}

GLCanvas3D* Plater::canvas3D()
{
    // BBS modify view3D->get_canvas3d() to current canvas
    return p->get_current_canvas3D();
}

const GLCanvas3D* Plater::canvas3D() const
{
    // BBS modify view3D->get_canvas3d() to current canvas
    return p->get_current_canvas3D();
}

GLCanvas3D* Plater::get_view3D_canvas3D()
{
    return p->view3D->get_canvas3d();
}

GLCanvas3D* Plater::get_preview_canvas3D()
{
    return p->preview->get_canvas3d();
}

GLCanvas3D* Plater::get_assmeble_canvas3D()
{
    if (p->assemble_view)
        return p->assemble_view->get_canvas3d();
    return nullptr;
}

GLCanvas3D* Plater::get_current_canvas3D(bool exclude_preview)
{
    return p->get_current_canvas3D(exclude_preview);
}

void Plater::arrange()
{
    auto &w = get_ui_job_worker();
    if (w.is_idle()) {
        p->take_snapshot(_u8L("Arrange"));
        replace_job(w, std::make_unique<ArrangeJob>());
    }
}

void Plater::set_current_canvas_as_dirty()
{
    p->set_current_canvas_as_dirty();
}

void Plater::unbind_canvas_event_handlers()
{
    p->unbind_canvas_event_handlers();
}

void Plater::reset_canvas_volumes()
{
    p->reset_canvas_volumes();
}

PrinterTechnology Plater::printer_technology() const
{
    return p->printer_technology;
}

const DynamicPrintConfig * Plater::config() const { return p->config; }

bool Plater::set_printer_technology(PrinterTechnology printer_technology)
{
    p->printer_technology = printer_technology;
    bool ret = p->background_process.select_technology(printer_technology);
    if (ret) {
        // Update the active presets.
    }
    //FIXME for SLA synchronize
    //p->background_process.apply(Model)!

    if (printer_technology == ptSLA) {
        for (ModelObject* model_object : p->model.objects) {
            model_object->ensure_on_bed();
        }
    }

    p->label_btn_export = printer_technology == ptFFF ? L("Export G-code") : L("Export");
    p->label_btn_send   = printer_technology == ptFFF ? L("Send G-code")   : L("Send to printer");

    if (wxGetApp().mainframe != nullptr)
        wxGetApp().mainframe->update_menubar();

    p->sidebar->get_searcher().set_printer_technology(printer_technology);

    p->notification_manager->set_fff(printer_technology == ptFFF);
    p->notification_manager->set_slicing_progress_hidden();

    return ret;
}

void Plater::clear_before_change_mesh(int obj_idx)
{
    ModelObject* mo = model().objects[obj_idx];

    // If there are custom supports/seams/mmu/fuzzy skin segmentation, remove them. Fixed mesh
    // may be different and they would make no sense.
    bool paint_removed = false;
    for (ModelVolume* mv : mo->volumes) {
        paint_removed |= ! mv->supported_facets.empty() || ! mv->seam_facets.empty() || ! mv->mmu_segmentation_facets.empty() || !mv->fuzzy_skin_facets.empty();
        mv->supported_facets.reset();
        mv->seam_facets.reset();
        mv->mmu_segmentation_facets.reset();
        mv->fuzzy_skin_facets.reset();
    }
    if (paint_removed) {
        // snapshot_time is captured by copy so the lambda knows where to undo/redo to.
        get_notification_manager()->push_notification(
                    NotificationType::CustomSupportsAndSeamRemovedAfterRepair,
                    NotificationManager::NotificationLevel::PrintInfoNotificationLevel,
                    _u8L("Custom supports and color painting were removed before repairing."));
    }
}

void Plater::changed_mesh(int obj_idx)
{
    ModelObject* mo = model().objects[obj_idx];
    sla::reproject_points_and_holes(mo);
    update();
    p->object_list_changed();
    p->schedule_background_process();
}

void Plater::changed_object(ModelObject &object){
    assert(object.get_model() == &p->model); // is object from same model?
    object.invalidate_bounding_box();

    // recenter and re - align to Z = 0
    object.ensure_on_bed(p->printer_technology != ptSLA);

    if (p->printer_technology == ptSLA) {
        // Update the SLAPrint from the current Model, so that the reload_scene()
        // pulls the correct data, update the 3D scene.
        p->update_restart_background_process(true, false);
    } else
        p->view3D->reload_scene(false);

    // update print
    p->schedule_background_process();
        
    // Check outside bed
    GLCanvas3D* canvas = get_current_canvas3D();
    if (canvas)
        canvas->requires_check_outside_state();
}

void Plater::changed_object(int obj_idx)
{
    if (obj_idx < 0)
        return;
    ModelObject *object = p->model.objects[obj_idx];
    if (object == nullptr)
        return;
    changed_object(*object);
}

void Plater::changed_objects(const std::vector<size_t>& object_idxs)
{
    if (object_idxs.empty())
        return;

    for (size_t obj_idx : object_idxs) {
        if (obj_idx < p->model.objects.size()) {
            if (p->model.objects[obj_idx]->min_z() >= SINKING_Z_THRESHOLD)
                // re - align to Z = 0
                p->model.objects[obj_idx]->ensure_on_bed();
        }
    }
    if (this->p->printer_technology == ptSLA) {
        // Update the SLAPrint from the current Model, so that the reload_scene()
        // pulls the correct data, update the 3D scene.
        this->p->update_restart_background_process(true, false);
    }
    else {
        p->view3D->reload_scene(false);
        p->view3D->get_canvas3d()->update_instance_printable_state_for_objects(object_idxs);
    }

    // update print
    this->p->schedule_background_process();
}

void Plater::schedule_background_process(bool schedule/* = true*/)
{
    if (schedule)
        this->p->schedule_background_process();

    this->p->suppressed_backround_processing_update = false;
}

bool Plater::is_background_process_update_scheduled() const
{
    return this->p->background_process_timer.IsRunning();
}

void Plater::suppress_background_process(const bool stop_background_process)
{
    if (stop_background_process)
        this->p->background_process_timer.Stop();

    this->p->suppressed_backround_processing_update = true;
}

void Plater::center_selection()     { p->center_selection(); }
void Plater::drop_selection()       { p->drop_selection(); }
void Plater::mirror(Axis axis)      { p->mirror(axis); }
void Plater::split_object()         { p->split_object(); }
void Plater::split_volume()         { p->split_volume(); }
void Plater::optimize_rotation()
{
    auto &w = get_ui_job_worker();
    if (w.is_idle()) {
        p->take_snapshot(_u8L("Optimize Rotation"));
        replace_job(w, std::make_unique<OrientJob>());
    }
}
void Plater::update_menus()         { p->menus.update(); }
// BBS
//void Plater::show_action_buttons(const bool ready_to_slice) const   { p->show_action_buttons(ready_to_slice); }

void Plater::fill_color(int extruder_id)
{
    if (can_fillcolor()) {
        p->assemble_view->get_canvas3d()->get_selection().fill_color(extruder_id);
    }
}

//BBS
void Plater::cut_selection_to_clipboard()
{
    Plater::TakeSnapshot snapshot(this, "Cut Selected Objects");
    if (can_cut_to_clipboard() && !p->sidebar->obj_list()->cut_to_clipboard()) {
        p->view3D->get_canvas3d()->get_selection().cut_to_clipboard();
    }
}

void Plater::copy_selection_to_clipboard()
{
    // At first try to copy selected values to the ObjectList's clipboard
    // to check if Settings or Layers are selected in the list
    // and then copy to 3DCanvas's clipboard if not
    if (can_copy_to_clipboard() && !p->sidebar->obj_list()->copy_to_clipboard())
        p->view3D->get_canvas3d()->get_selection().copy_to_clipboard();
}

void Plater::paste_from_clipboard()
{
    if (!can_paste_from_clipboard())
        return;

    Plater::TakeSnapshot snapshot(this, "Paste From Clipboard");

    // At first try to paste values from the ObjectList's clipboard
    // to check if Settings or Layers were copied
    // and then paste from the 3DCanvas's clipboard if not
    if (!p->sidebar->obj_list()->paste_from_clipboard())
        p->view3D->get_canvas3d()->get_selection().paste_from_clipboard();
}

//BBS: add clone
void Plater::clone_selection()
{
    if (is_selection_empty())
        return;
    CloneDialog dlg(this);
    dlg.ShowModal();
}

std::vector<Vec2f> Plater::get_empty_cells(const Vec2f step)
{
    PartPlate* plate = wxGetApp().plater()->get_partplate_list().get_curr_plate();
    BoundingBoxf3 build_volume = plate->get_build_volume();
    Vec2d vmin(build_volume.min.x(), build_volume.min.y()), vmax(build_volume.max.x(), build_volume.max.y());
    BoundingBoxf bbox(vmin, vmax);
    std::vector<Vec2f> cells;
    auto min_x = step(0)/2;// start_point.x() - step(0) * int((start_point.x() - bbox.min.x()) / step(0));
    auto min_y = step(1)/2;// start_point.y() - step(1) * int((start_point.y() - bbox.min.y()) / step(1));
    auto& exclude_box3s = plate->get_exclude_areas();
    std::vector<BoundingBoxf> exclude_boxs;
    for (auto& box : exclude_box3s) {
        Vec2d vmin(box.min.x(), box.min.y()), vmax(box.max.x(), box.max.y());
        exclude_boxs.emplace_back(vmin, vmax);
    }
    for (float x = min_x + bbox.min.x(); x < bbox.max.x() - step(0) / 2; x += step(0))
        for (float y = min_y + bbox.min.y(); y < bbox.max.y() - step(1) / 2; y += step(1)) {
            bool in_exclude = false;
            BoundingBoxf cell(Vec2d(x - step(0) / 2, y - step(1) / 2), Vec2d(x + step(0) / 2, y + step(1) / 2));
            for (auto& box : exclude_boxs) {
                if (box.overlap(cell)) {
                    in_exclude = true;
                    break;
                }
            }
            if(in_exclude)
                continue;
            cells.emplace_back(x, y);
        }
    return cells;
}

void Plater::search(bool plater_is_active, Preset::Type type, wxWindow *tag, TextInput *etag, wxWindow *stag)
{
    if (plater_is_active) {
        if (is_preview_shown())
            return;
        // plater should be focused for correct navigation inside search window
        this->SetFocus();

        wxKeyEvent evt;
#ifdef __APPLE__
        evt.m_keyCode = 'f';
#else /* __APPLE__ */
        evt.m_keyCode = WXK_CONTROL_F;
#endif /* __APPLE__ */
        evt.SetControlDown(true);
        canvas3D()->on_char(evt);
    }
    else
        p->sidebar->get_searcher().show_dialog(type, tag, etag, stag);
}

void Plater::msw_rescale()
{
    p->preview->msw_rescale();

    p->view3D->get_canvas3d()->msw_rescale();

    p->sidebar->msw_rescale();

    p->menus.msw_rescale();

    Layout();
    GetParent()->Layout();
}

void Plater::sys_color_changed()
{
    p->preview->sys_color_changed();
    p->sidebar->sys_color_changed();
    p->menus.sys_color_changed();
    if (p->m_select_machine_dlg) p->m_select_machine_dlg->sys_color_changed();

    Layout();
    GetParent()->Layout();
}

// BBS
#if 0
bool Plater::init_view_toolbar()
{
    return p->init_view_toolbar();
}

void Plater::enable_view_toolbar(bool enable)
{
    p->view_toolbar.set_enabled(enable);
}
#endif

bool Plater::init_collapse_toolbar()
{
    return p->init_collapse_toolbar();
}

const Camera& Plater::get_camera() const
{
    return p->camera;
}

Camera& Plater::get_camera()
{
    return p->camera;
}

//BBS: partplate list related functions
PartPlateList& Plater::get_partplate_list()
{
    return p->partplate_list;
}

void Plater::apply_background_progress()
{
    PartPlate* part_plate = p->partplate_list.get_curr_plate();
    int plate_index = p->partplate_list.get_curr_plate_index();
    bool result_valid = part_plate->is_slice_result_valid();
    //always apply the current plate's print
    Print::ApplyStatus invalidated = p->background_process.apply(this->model(), wxGetApp().preset_bundle->full_config());
    p->notify_filament_compatibility_after_apply();

    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(" %1%: plate %2%, after apply, invalidated= %3%, previous result_valid %4% ") % __LINE__ % plate_index % invalidated % result_valid;
    if (invalidated & PrintBase::APPLY_STATUS_INVALIDATED)
    {
        part_plate->update_slice_result_valid_state(false);
        //p->ready_to_slice = true;
        if (p->can_current_plate_be_sliced())
            p->main_frame->update_slice_print_status(MainFrame::eEventPlateUpdate, true);
        else
            p->main_frame->update_slice_print_status(MainFrame::eEventPlateUpdate, false);
    }
}

//BBS: select Plate
int Plater::select_plate(int plate_index, bool need_slice)
{
    int ret;
    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(" %1%: plate %2%, need_slice %3% ")%__LINE__ %plate_index  %need_slice;
    take_snapshot("select partplate!");
    ret = p->partplate_list.select_plate(plate_index);
    if (!ret) {
        if (is_view3D_shown())
            wxGetApp().plater()->canvas3D()->render();
    }

    if ((!ret) && (p->background_process.can_switch_print()))
    {
        //select successfully
        p->partplate_list.update_slice_context_to_current_plate(p->background_process);
        p->preview->update_gcode_result(p->partplate_list.get_current_slice_result());
        p->update_print_volume_state();

        PartPlate* part_plate = p->partplate_list.get_curr_plate();
        bool result_valid = part_plate->is_slice_result_valid();
        PrintBase* print = nullptr;
        GCodeResult* gcode_result = nullptr;
        Print::ApplyStatus invalidated;

        part_plate->get_print(&print, &gcode_result, NULL);

        //always apply the current plate's print
        invalidated = p->background_process.apply(this->model(), wxGetApp().preset_bundle->full_config());
        p->notify_filament_compatibility_after_apply();
        bool model_fits, validate_err;

        BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(" %1%: plate %2%, after apply, invalidated= %3%, previous result_valid %4% ")%__LINE__ %plate_index  %invalidated %result_valid;
        if (result_valid)
        {
            if (is_preview_shown())
            {
                if (need_slice) { //from preview's thumbnail
                    if ((invalidated & PrintBase::APPLY_STATUS_INVALIDATED) || (gcode_result->moves.empty())){
                        if (invalidated & PrintBase::APPLY_STATUS_INVALIDATED)
                            part_plate->update_slice_result_valid_state(false);
                        p->process_completed_with_error = -1;
                        p->m_slice_all = false;
                        reset_gcode_toolpaths();
                        if (!guard_before_slice_plate())
                            return ret;
                        if (!reslice())
                            return ret;
                    }
                    else {
                        validate_current_plate(model_fits, validate_err);
                        //just refresh_print
                        refresh_print();
                        p->main_frame->update_slice_print_status(MainFrame::eEventPlateUpdate, false, true);
                    }
                }
                else {// from multiple slice's next
                    //do nothing
                }
            }
            else
            {
                validate_current_plate(model_fits, validate_err);
                if (invalidated & PrintBase::APPLY_STATUS_INVALIDATED)
                {
                    part_plate->update_slice_result_valid_state(false);
                    // BBS
                    //p->show_action_buttons(true);
                    //p->ready_to_slice = true;
                    if (p->can_current_plate_be_sliced())
                        p->main_frame->update_slice_print_status(MainFrame::eEventPlateUpdate, true);
                    else
                        p->main_frame->update_slice_print_status(MainFrame::eEventPlateUpdate, false);
                }
                else
                {
                    // BBS
                    //p->show_action_buttons(false);
                    //p->ready_to_slice = false;
                    p->main_frame->update_slice_print_status(MainFrame::eEventPlateUpdate, false);

                    refresh_print();
                }
            }
        }
        else
        {
            //check inside status
            //model_fits = p->view3D->get_canvas3d()->check_volumes_outside_state() != ModelInstancePVS_Partly_Outside;
            //bool validate_err = false;
            validate_current_plate(model_fits, validate_err);
            if (model_fits && !validate_err) {
                p->process_completed_with_error = -1;
            }
            else {
                p->process_completed_with_error = p->partplate_list.get_curr_plate_index();
            }
            if (is_preview_shown())
            {
                if (need_slice)
                {
                    //p->process_completed_with_error = -1;
                    p->m_slice_all = false;
                    reset_gcode_toolpaths();
                    if (model_fits && !validate_err)
                    {
                        if (!guard_before_slice_plate())
                            return ret;
                        if (!reslice())
                            return ret;
                    }
                    else
                    {
                        p->main_frame->update_slice_print_status(MainFrame::eEventPlateUpdate, false);
                        //sometimes the previous print's sliced result is still valid, but the newly added object is laid over the boundary
                        //then the print toolpath will be shown, so we should not refresh print here, only onload shell
                        //refresh_print();
                        p->update_fff_scene_only_shells();
                    }
                }
                else {
                    //p->ready_to_slice = false;
                    p->main_frame->update_slice_print_status(MainFrame::eEventPlateUpdate, false);
                    refresh_print();
                }
            }
            else
            {
                //validate_current_plate(model_fits, validate_err);
                //check inside status
                /*if (model_fits && !validate_err){
                    p->process_completed_with_error = -1;
                }
                else {
                    p->process_completed_with_error = p->partplate_list.get_curr_plate_index();
                }*/

                // BBS: don't show action buttons
                //p->show_action_buttons(true);
                //p->ready_to_slice = true;
                if (model_fits && part_plate->has_printable_instances()
                    && p->can_current_plate_be_sliced())
                {
                    //p->view3D->get_canvas3d()->post_event(Event<bool>(EVT_GLCANVAS_ENABLE_ACTION_BUTTONS, true));
                    p->main_frame->update_slice_print_status(MainFrame::eEventPlateUpdate, true);
                }
                else
                {
                    //p->view3D->get_canvas3d()->post_event(Event<bool>(EVT_GLCANVAS_ENABLE_ACTION_BUTTONS, false));
                    p->main_frame->update_slice_print_status(MainFrame::eEventPlateUpdate, false);
                }
            }
        }
    }

    SimpleEvent event(EVT_GLCANVAS_PLATE_SELECT);
    p->on_plate_selected(event);
    sync_filament_temp_mixing_notification();
    sync_flow_ratio_zero_notification();
    sync_cold_plate_notification();

    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(" %1%: plate %2%, return %3%")%__LINE__ %plate_index %ret;
    return ret;
}

int Plater::select_sliced_plate(int plate_index)
{
    int ret = 0;
    BOOST_LOG_TRIVIAL(info) << "select_sliced_plate plate_idx=" << plate_index;

    Freeze();
    ret = select_plate(plate_index, true);
    if (ret)
    {
        BOOST_LOG_TRIVIAL(error) << "select_plate error for plate_idx=" << plate_index;
        Thaw();
        return -1;
    }
    p->partplate_list.select_plate_view();
    Thaw();

    return ret;
}

void Plater::validate_current_plate(bool& model_fits, bool& validate_error)
{
    model_fits = p->view3D->get_canvas3d()->check_volumes_outside_state() != ModelInstancePVS_Partly_Outside;
    validate_error = false;
    if (p->printer_technology == ptFFF) {
        std::string plater_text = _u8L("An object is laid over the boundary of plate or exceeds the height limit.\n"
                    "Please solve the problem by moving it totally on or off the plate, and confirming that the height is within the build volume.");;
        StringObjectException warning;
        Polygons polygons;
        std::vector<std::pair<Polygon, float>> height_polygons;
        StringObjectException err = p->background_process.validate(&warning, &polygons, &height_polygons);
        // update string by type
        post_process_string_object_exception(err);
        BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": validate err=%1%, warning=%2%, model_fits %3%")%err.string%warning.string %model_fits;

        if (err.string.empty()) {
            // Print::validate() passed, but GUI-layer blockers (filament temp
            // mixing, cold plate incompatibility) may still be active. Their
            // ValidateError notifications must survive this close-all — gate it.
            const int curr_plate_idx = p->partplate_list.get_curr_plate_index();
            const bool blocked_by_mixing = is_plate_blocked_by_filament_temp_mixing(curr_plate_idx);
            const bool blocked_by_cold    = is_plate_blocked_by_cold_plate(curr_plate_idx);

            p->partplate_list.get_curr_plate()->update_apply_result_invalid(false);
            p->notification_manager->set_all_slicing_errors_gray(true);
            if (!blocked_by_mixing && !blocked_by_cold) {
                p->notification_manager->close_notification_of_type(NotificationType::ValidateError);
            }

            // Pass a warning from validation and either show a notification,
            // or hide the old one.
            p->process_validation_warning(warning);
            p->view3D->get_canvas3d()->reset_sequential_print_clearance();
            p->view3D->get_canvas3d()->set_as_dirty();
            p->view3D->get_canvas3d()->request_extra_frame();
        }
        else {
            // The print is not valid.
            p->partplate_list.get_curr_plate()->update_apply_result_invalid(true);
            // Show error as notification.
            p->notification_manager->push_validate_error_notification(err);
            p->process_validation_warning(warning);
            //model_fits = false;
            validate_error = true;
            p->view3D->get_canvas3d()->set_sequential_print_clearance_visible(true);
            p->view3D->get_canvas3d()->set_sequential_print_clearance_render_fill(true);
            p->view3D->get_canvas3d()->set_sequential_print_clearance_polygons(polygons, height_polygons);
        }

        if (!model_fits) {
            p->notification_manager->push_plater_error_notification(plater_text);
        }
        else {
            p->notification_manager->close_plater_error_notification(plater_text);
        }

        sync_filament_temp_mixing_notification();
        sync_flow_ratio_zero_notification();
        sync_cold_plate_notification();
    }

    PartPlate* part_plate = p->partplate_list.get_curr_plate();
    part_plate->update_slice_ready_status(model_fits);

    return;
}

void Plater::open_platesettings_dialog(wxCommandEvent& evt) {
    int plate_index = evt.GetInt();
    PlateSettingsDialog dlg(this, _L("Plate Settings"), evt.GetString() == "only_layer_sequence");
    PartPlate* curr_plate = p->partplate_list.get_curr_plate();
    dlg.sync_bed_type(curr_plate->get_bed_type(true));

    auto curr_print_seq = curr_plate->get_print_seq();
    if (curr_print_seq != PrintSequence::ByDefault) {
        dlg.sync_print_seq(int(curr_print_seq) + 1);
    }
    else
        dlg.sync_print_seq(0);

    auto first_layer_print_seq = curr_plate->get_first_layer_print_sequence();
    if (first_layer_print_seq.empty())
        dlg.sync_first_layer_print_seq(0);
    else
        dlg.sync_first_layer_print_seq(1, curr_plate->get_first_layer_print_sequence());

    auto other_layers_print_seq = curr_plate->get_other_layers_print_sequence();
    if (other_layers_print_seq.empty())
        dlg.sync_other_layers_print_seq(0, {});
    else {
        dlg.sync_other_layers_print_seq(1, curr_plate->get_other_layers_print_sequence());
    }

    dlg.sync_spiral_mode(curr_plate->get_spiral_vase_mode(), !curr_plate->has_spiral_mode_config());

    dlg.Bind(EVT_SET_BED_TYPE_CONFIRM, [this, plate_index, &dlg](wxCommandEvent& e) {
        PartPlate* curr_plate = p->partplate_list.get_curr_plate();
        BedType old_bed_type = curr_plate->get_bed_type(true);
        auto bt_sel = BedType(dlg.get_bed_type_choice());
        if (old_bed_type != bt_sel) {
            curr_plate->set_bed_type(bt_sel);
            update_project_dirty_from_presets();
            set_plater_dirty(true);
        }
        BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format("select bed type %1% for plate %2% at plate side") % bt_sel % plate_index;

        if (dlg.get_first_layer_print_seq_choice() != 0)
            curr_plate->set_first_layer_print_sequence(dlg.get_first_layer_print_seq());
        else
            curr_plate->set_first_layer_print_sequence({});

        if (dlg.get_other_layers_print_seq_choice() != 0)
            curr_plate->set_other_layers_print_sequence(dlg.get_other_layers_print_seq_infos());
        else
            curr_plate->set_other_layers_print_sequence({});

        int ps_sel = dlg.get_print_seq_choice();
        if (ps_sel != 0)
            curr_plate->set_print_seq(PrintSequence(ps_sel - 1));
        else
            curr_plate->set_print_seq(PrintSequence::ByDefault);

        int spiral_sel = dlg.get_spiral_mode_choice();
        if (spiral_sel == 1) {
            curr_plate->set_spiral_vase_mode(true, false);
        }
        else if (spiral_sel == 2) {
            curr_plate->set_spiral_vase_mode(false, false);
        }
        else {
            curr_plate->set_spiral_vase_mode(false, true);
        }

        update_project_dirty_from_presets();
        set_plater_dirty(true);
        BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format("select print sequence %1% for plate %2% at plate side") % ps_sel % plate_index;
        auto plate_config = *(curr_plate->config());
        wxGetApp().plater()->config_change_notification(plate_config, std::string("print_sequence"));
        update();
        wxGetApp().obj_list()->update_selections();
        });
    dlg.set_plate_name(from_u8(curr_plate->get_plate_name()));

    dlg.ShowModal();
    curr_plate->set_plate_name(dlg.get_plate_name().ToUTF8().data());
}

//BBS: select Plate by hover_id
int Plater::select_plate_by_hover_id(int hover_id, bool right_click, bool isModidyPlateName)
{
    int ret;
    int action, plate_index;

    plate_index = hover_id / PartPlate::GRABBER_COUNT;
    action      = isModidyPlateName ? PartPlate::PLATE_NAME_HOVER_ID : hover_id % PartPlate::GRABBER_COUNT;

    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": enter, hover_id %1%, plate_index %2%, action %3%")%hover_id % plate_index %action;
    if (action == 0)
    {
        //select plate
        ret = p->partplate_list.select_plate(plate_index);
        if (!ret) {
            SimpleEvent event(EVT_GLCANVAS_PLATE_SELECT);
            p->on_plate_selected(event);
        }
        if ((!ret)&&(p->background_process.can_switch_print()))
        {
            //select successfully
            p->partplate_list.update_slice_context_to_current_plate(p->background_process);
            p->preview->update_gcode_result(p->partplate_list.get_current_slice_result());
            p->update_print_volume_state();

            PartPlate* part_plate = p->partplate_list.get_curr_plate();
            bool result_valid = part_plate->is_slice_result_valid();
            PrintBase* print = nullptr;
            GCodeResult* gcode_result = nullptr;
            Print::ApplyStatus invalidated;

            part_plate->get_print(&print, &gcode_result, NULL);
            //always apply the current plate's print
            invalidated = p->background_process.apply(this->model(), wxGetApp().preset_bundle->full_config());
            p->notify_filament_compatibility_after_apply();
            bool model_fits, validate_err;
            validate_current_plate(model_fits, validate_err);

            BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(" %1%: after apply, invalidated= %2%, previous result_valid %3% ")%__LINE__ % invalidated %result_valid;
            if (result_valid)
            {
                if (invalidated & PrintBase::APPLY_STATUS_INVALIDATED)
                {
                    //bool model_fits, validate_err;
                    //validate_current_plate(model_fits, validate_err);
                    part_plate->update_slice_result_valid_state(false);

                    // BBS
                    //p->show_action_buttons(true);
                    //p->ready_to_slice = true;
                    if (p->can_current_plate_be_sliced())
                        p->main_frame->update_slice_print_status(MainFrame::eEventPlateUpdate, true);
                    else
                        p->main_frame->update_slice_print_status(MainFrame::eEventPlateUpdate, false);
                }
                else
                {
                    // BBS
                    //p->show_action_buttons(false);
                    //validate_current_plate(model_fits, validate_err);
                    //p->ready_to_slice = false;
                    p->main_frame->update_slice_print_status(MainFrame::eEventPlateUpdate, false);

                    refresh_print();
                }
            }
            else
            {
                //check inside status
                if (model_fits && !validate_err){
                    p->process_completed_with_error = -1;
                }
                else {
                    p->process_completed_with_error = p->partplate_list.get_curr_plate_index();
                }

                // BBS: don't show action buttons
                //p->show_action_buttons(true);
                //p->ready_to_slice = true;
                if (model_fits && part_plate->has_printable_instances()
                    && p->can_current_plate_be_sliced())
                {
                    //p->view3D->get_canvas3d()->post_event(Event<bool>(EVT_GLCANVAS_ENABLE_ACTION_BUTTONS, true));
                    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": will set can_slice to true");
                    p->main_frame->update_slice_print_status(MainFrame::eEventPlateUpdate, true);
                }
                else
                {
                    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": will set can_slice to false, has_printable_instances %1%")%part_plate->has_printable_instances();
                    //p->view3D->get_canvas3d()->post_event(Event<bool>(EVT_GLCANVAS_ENABLE_ACTION_BUTTONS, false));
                    p->main_frame->update_slice_print_status(MainFrame::eEventPlateUpdate, false);
                }
            }
        }
    }
    else if ((action == 1)&&(!right_click))
    {
        //delete plate
        ret = delete_plate(plate_index);
    }
    else if ((action == 2)&&(!right_click))
    {
        //arrange the plate
        //take_snapshot("select_orient partplate");
        ret = select_plate(plate_index);
        if (!ret)
        {
            set_prepare_state(Job::PREPARE_STATE_MENU);
            orient();
        }
        else
        {
            BOOST_LOG_TRIVIAL(error) << __FUNCTION__ << "can not select plate %1%" << plate_index;
            ret = -1;
        }
    }
    else if ((action == 3)&&(!right_click))
    {
        //arrange the plate
        //take_snapshot("select_arrange partplate");
        ret = select_plate(plate_index);
        if (!ret)
        {
            if (last_arrange_job_is_finished()) {
                set_prepare_state(Job::PREPARE_STATE_MENU);
                arrange();
            }
        }
        else
        {
            BOOST_LOG_TRIVIAL(error) << __FUNCTION__ << "can not select plate %1%" << plate_index;
            ret = -1;
        }
    }
    else if ((action == 4)&&(!right_click))
    {
        //lock the plate
        take_snapshot("lock partplate");
        ret = p->partplate_list.lock_plate(plate_index, !p->partplate_list.is_locked(plate_index));
    }
    else if ((action == 5)&&(!right_click))
    {
        // set the plate type
        ret = select_plate(plate_index);
        if (!ret) {
            wxCommandEvent evt(EVT_OPEN_PLATESETTINGSDIALOG);
            evt.SetInt(plate_index);
            evt.SetEventObject(this);
            wxPostEvent(this, evt);

            this->schedule_background_process();
        } else {
            BOOST_LOG_TRIVIAL(error) << __FUNCTION__ << "can not select plate %1%" << plate_index;
            ret = -1;
        }
    }
    else if ((action == 6) && (!right_click)) {
        // set the plate type
        ret = select_plate(plate_index);
        if (!ret) {
            PlateNameEditDialog dlg(this, wxID_ANY, _L("Edit Plate Name"));
            PartPlate *         curr_plate = p->partplate_list.get_curr_plate();

            wxString curr_plate_name = from_u8(curr_plate->get_plate_name());
            dlg.set_plate_name(curr_plate_name);

            int result=dlg.ShowModal();
            if (result == wxID_YES) {
                wxString dlg_plate_name = dlg.get_plate_name();
                curr_plate->set_plate_name(dlg_plate_name.ToUTF8().data());
            }
        } else {
            BOOST_LOG_TRIVIAL(error) << __FUNCTION__ << "can not select plate %1%" << plate_index;
            ret = -1;
        }
    } else if ((action == 7) && (!right_click)) {
        // move plate to the front
        take_snapshot("move plate to the front");
        ret = p->partplate_list.move_plate_to_index(plate_index,0);
        p->partplate_list.update_slice_context_to_current_plate(p->background_process);
        p->preview->update_gcode_result(p->partplate_list.get_current_slice_result());
        p->sidebar->obj_list()->reload_all_plates();
        p->partplate_list.update_plates();
        update();
        p->partplate_list.select_plate(0);
    }

    else
    {
        BOOST_LOG_TRIVIAL(error) << __FUNCTION__ << "invalid action %1%, with right_click=%2%" << action << right_click;
        ret = -1;
    }

    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(" %1%: return %2%")%__LINE__ % ret;
    return ret;
}

int Plater::duplicate_plate(int plate_index)
{
    int index = plate_index, ret;
    if (plate_index == -1)
        index = p->partplate_list.get_curr_plate_index();

    ret = p->partplate_list.duplicate_plate(index);

    //need to call update
    update();
    return ret;
}

//BBS: delete the plate, index= -1 means the current plate
int Plater::delete_plate(int plate_index)
{
    int index = plate_index, ret;

    if (plate_index == -1)
        index = p->partplate_list.get_curr_plate_index();

    take_snapshot("delete partplate");

    // CRASH FIX: Clear fff_print reference before PartPlateList::delete_plate destroys the Print,
    // preventing dangling pointer access during subsequent update calls.
    p->background_process.set_fff_print(nullptr);

    ret = p->partplate_list.delete_plate(index);

    //BBS: update the current print to the current plate
    p->partplate_list.update_slice_context_to_current_plate(p->background_process);
    p->preview->update_gcode_result(p->partplate_list.get_current_slice_result());
    p->sidebar->obj_list()->reload_all_plates();

    // BBS update default view
    //get_camera().select_view("topfront");
    //get_camera().requires_zoom_to_plate = REQUIRES_ZOOM_TO_ALL_PLATE;

    //need to call update
    update();
    return ret;
}

//BBS: set bed positions
void Plater::set_bed_position(Vec2d& pos)
{
    p->bed.set_position(pos);
}

//BBS: is the background process slicing currently
bool Plater::is_background_process_slicing() const
{
    return p->m_is_slicing;
}

//BBS: update slicing context
void Plater::update_slicing_context_to_current_partplate()
{
    p->partplate_list.update_slice_context_to_current_plate(p->background_process);
    p->preview->update_gcode_result(p->partplate_list.get_current_slice_result());
}

//BBS: show object info
void Plater::show_object_info()
{
    NotificationManager *notify_manager = get_notification_manager();
    const Selection& selection = get_selection();
    int selCount = selection.get_volume_idxs().size();
    ModelObjectPtrs objects = model().objects;
    int obj_idx = selection.get_object_idx();
    std::string info_text;

    if (selCount > 1 && !selection.is_single_full_object()) {
        notify_manager->bbl_close_objectsinfo_notification();
        if (selection.get_mode() == Selection::EMode::Volume) {
            info_text += (boost::format(_utf8(L("Number of currently selected parts: %1%\n"))) % selCount).str();
        } else if (selection.get_mode() == Selection::EMode::Instance) {
            int content_count = selection.get_content().size();
            info_text += (boost::format(_utf8(L("Number of currently selected objects: %1%\n"))) % content_count).str();
        }
        notify_manager->bbl_show_objectsinfo_notification(info_text, false, !(p->current_panel == p->view3D));
        return;
    }
    else if (objects.empty() || (obj_idx < 0) || (obj_idx >= objects.size()) ||
        objects[obj_idx]->volumes.empty() ||// hack to avoid crash when deleting the last object on the bed
        (selection.is_single_full_object() && objects[obj_idx]->instances.size()> 1) ||
        !(selection.is_single_full_instance() || selection.is_single_volume()))
    {
        notify_manager->bbl_close_objectsinfo_notification();
        return;
    }

    const ModelObject* model_object = objects[obj_idx];
    int inst_idx = selection.get_instance_idx();
    if ((inst_idx < 0) || (inst_idx >= model_object->instances.size()))
    {
        notify_manager->bbl_close_objectsinfo_notification();
        return;
    }
    bool imperial_units = wxGetApp().app_config->get("use_inches") == "1";
    double koef = imperial_units ? GizmoObjectManipulation::mm_to_in : 1.0f;

    ModelVolume* vol = nullptr;
    Transform3d t;
    int face_count;
    Vec3d size;
    if (selection.is_single_volume()) {
        std::vector<int> obj_idxs, vol_idxs;
        wxGetApp().obj_list()->get_selection_indexes(obj_idxs, vol_idxs);
        if (vol_idxs.size() != 1)
        {
            //corner case when merge/split/remove
            return;
        }
        vol = model_object->volumes[vol_idxs[0]];
        t = model_object->instances[inst_idx]->get_matrix() * vol->get_matrix();
        info_text += (boost::format(_utf8(L("Part name: %1%\n"))) % vol->name).str();
        face_count = static_cast<int>(vol->mesh().facets_count());
        size = vol->get_convex_hull().transformed_bounding_box(t).size();
    }
    else {
        //int obj_idx, vol_idx;
        //wxGetApp().obj_list()->get_selected_item_indexes(obj_idx, vol_idx);
        //if (obj_idx < 0) {
        //    //corner case when merge/split/remove
        //    return;
        //}
        info_text += (boost::format(_utf8(L("Object name: %1%\n"))) % model_object->name).str();
        face_count = static_cast<int>(model_object->facets_count());
        size = model_object->instance_convex_hull_bounding_box(inst_idx).size();
    }

    //Vec3d size = vol ? vol->mesh().transformed_bounding_box(t).size() : model_object->instance_bounding_box(inst_idx).size();
    if (imperial_units)
        info_text += (boost::format(_utf8(L("Size: %1% x %2% x %3% in\n"))) %(size(0)*koef) %(size(1)*koef) %(size(2)*koef)).str();
    else
        info_text += (boost::format(_utf8(L("Size: %1% x %2% x %3% mm\n"))) %size(0) %size(1) %size(2)).str();

    const TriangleMeshStats& stats = vol ? vol->mesh().stats() : model_object->get_object_stl_stats();
    double volume_val = stats.volume;
    if (vol)
        volume_val *= std::fabs(t.matrix().block(0, 0, 3, 3).determinant());
    volume_val = volume_val * pow(koef,3);
    if (imperial_units)
        info_text += (boost::format(_utf8(L("Volume: %1% in³\n"))) %volume_val).str();
    else
        info_text += (boost::format(_utf8(L("Volume: %1% mm³\n"))) %volume_val).str();
    info_text += (boost::format(_utf8(L("Triangles: %1%\n"))) %face_count).str();

    wxString info_manifold;
    int non_manifold_edges = 0;
    auto mesh_errors = p->sidebar->obj_list()->get_mesh_errors_info(&info_manifold, &non_manifold_edges);

    #ifndef __WINDOWS__
    if (non_manifold_edges > 0) {
        info_manifold += into_u8("\n" + _L("Tips:") + "\n" +_L("\"Fix Model\" feature is currently only on Windows. Please repair the model on Snapmaker Orca(windows) or CAD softwares."));
    }
    #endif //APPLE & LINUX

    info_manifold = "<Error>" + info_manifold + "</Error>";
    info_text += into_u8(info_manifold);
    notify_manager->bbl_show_objectsinfo_notification(info_text, is_windows10()&&(non_manifold_edges > 0), !(p->current_panel == p->view3D));
}

bool Plater::show_publish_dialog(bool show)
{
    return p->show_publish_dlg(show);
}

void Plater::post_process_string_object_exception(StringObjectException &err)
{
    PresetBundle* preset_bundle = wxGetApp().preset_bundle;
    if (err.type == StringExceptionType::STRING_EXCEPT_FILAMENT_NOT_MATCH_BED_TYPE) {
        try {
            int extruder_id = atoi(err.params[2].c_str()) - 1;
            if (extruder_id < preset_bundle->filament_presets.size()) {
                std::string filament_name = preset_bundle->filament_presets[extruder_id];
                for (auto filament_it = preset_bundle->filaments.begin(); filament_it != preset_bundle->filaments.end(); filament_it++) {
                    if (filament_it->name == filament_name) {
                        if (filament_it->is_system) {
                            filament_name = filament_it->alias;
                        } else {
                            auto preset = preset_bundle->filaments.get_preset_base(*filament_it);
                            if (preset && !preset->alias.empty()) {
                                filament_name = preset->alias;
                            } else {
                                char target = '@';
                                size_t pos    = filament_name.find(target);
                                if (pos != std::string::npos) {
                                    filament_name = filament_name.substr(0, pos - 1);
                                }
                            }
                        }
                        break;
                    }
                }
                err.string = format(_L("Plate %d: %s is not suggested to be used to print filament %s (%s). "
                                       "If you still want to do this print job, please set this filament's bed temperature to non-zero."),
                             err.params[0], err.params[1], err.params[2], filament_name);
                err.string += "\n";
            }
        } catch (...) {
            ;
        }
    }

    return;
}

#if ENABLE_ENVIRONMENT_MAP
void Plater::init_environment_texture()
{
    if (p->environment_texture.get_id() == 0)
        p->environment_texture.load_from_file(resources_dir() + "/images/Pmetal_001.png", false, GLTexture::SingleThreaded, false);
}

unsigned int Plater::get_environment_texture_id() const
{
    return p->environment_texture.get_id();
}
#endif // ENABLE_ENVIRONMENT_MAP

const BuildVolume& Plater::build_volume() const
{
    return p->bed.build_volume();
}

// BBS
#if 0
const GLToolbar& Plater::get_view_toolbar() const
{
    return p->view_toolbar;
}

GLToolbar& Plater::get_view_toolbar()
{
    return p->view_toolbar;
}
#endif

const GLToolbar& Plater::get_collapse_toolbar() const
{
    return p->collapse_toolbar;
}

GLToolbar& Plater::get_collapse_toolbar()
{
    return p->collapse_toolbar;
}

void Plater::update_preview_bottom_toolbar()
{
    p->update_preview_bottom_toolbar();
}

void Plater::reset_gcode_toolpaths()
{
    //BBS: add some logs
    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(": reset the gcode viewer's toolpaths");
    p->reset_gcode_toolpaths();
}

void Plater::post_slice_state_change_update()
{
    p->partplate_list.invalid_all_slice_result();
    reset_gcode_toolpaths();
    wxGetApp().mainframe->update_slice_print_status(MainFrame::SlicePrintEventType::eEventSliceUpdate, true, false);
    CallAfter([this]() {
        GLCanvas3D* canvas = get_current_canvas3D();
        if (canvas) {
            canvas->set_as_dirty();
            canvas->request_extra_frame();
        }
    });
}

const Mouse3DController& Plater::get_mouse3d_controller() const
{
    return p->mouse3d_controller;
}

Mouse3DController& Plater::get_mouse3d_controller()
{
    return p->mouse3d_controller;
}

NotificationManager * Plater::get_notification_manager()
{
    return p->notification_manager.get();
}

DailyTipsWindow* Plater::get_dailytips() const
{
    static DailyTipsWindow* dailytips_win = new DailyTipsWindow();
    return dailytips_win;
}

const NotificationManager * Plater::get_notification_manager() const
{
    return p->notification_manager.get();
}

void Plater::init_notification_manager()
{
    p->init_notification_manager();
}

void Plater::show_status_message(std::string s)
{
    BOOST_LOG_TRIVIAL(trace) << "show_status_message:" << s;
}

bool Plater::can_delete() const { return p->can_delete(); }
bool Plater::can_delete_all() const { return p->can_delete_all(); }
bool Plater::can_add_model() const { return !is_background_process_slicing(); }
bool Plater::can_add_plate() const { return !is_background_process_slicing() && p->can_add_plate(); }
bool Plater::can_delete_plate() const { return p->can_delete_plate(); }
bool Plater::can_increase_instances() const { return p->can_increase_instances(); }
bool Plater::can_decrease_instances() const { return p->can_decrease_instances(); }
bool Plater::can_set_instance_to_object() const { return p->can_set_instance_to_object(); }
bool Plater::can_fix_through_netfabb() const { return p->can_fix_through_netfabb(); }
bool Plater::can_simplify() const { return p->can_simplify(); }
bool Plater::can_split_to_objects() const { return p->can_split_to_objects(); }
bool Plater::can_split_to_volumes() const { return p->can_split_to_volumes(); }
bool Plater::can_arrange() const { return p->can_arrange(); }
bool Plater::can_layers_editing() const { return p->can_layers_editing(); }
bool Plater::can_paste_from_clipboard() const
{
    if (!IsShown() || !p->is_view3D_shown()) return false;

    const Selection& selection = p->view3D->get_canvas3d()->get_selection();
    const Selection::Clipboard& clipboard = selection.get_clipboard();

    if (clipboard.is_empty() && p->sidebar->obj_list()->clipboard_is_empty())
        return false;

    if ((wxGetApp().preset_bundle->printers.get_edited_preset().printer_technology() == ptSLA) && !clipboard.is_sla_compliant())
        return false;

    Selection::EMode mode = clipboard.get_mode();
    if ((mode == Selection::Volume) && !selection.is_from_single_instance())
        return false;

    if ((mode == Selection::Instance) && (selection.get_mode() != Selection::Instance))
        return false;

    return true;
}

//BBS support cut
bool Plater::can_cut_to_clipboard() const
{
    if (is_selection_empty())
        return false;
    return true;
}

bool Plater::can_copy_to_clipboard() const
{
    if (!IsShown() || !p->is_view3D_shown())
        return false;

    if (is_selection_empty())
        return false;

    const Selection& selection = p->view3D->get_canvas3d()->get_selection();
    if ((wxGetApp().preset_bundle->printers.get_edited_preset().printer_technology() == ptSLA) && !selection.is_sla_compliant())
        return false;

    return true;
}

bool Plater::can_undo() const { return IsShown() && p->is_view3D_shown() && p->undo_redo_stack().has_undo_snapshot(); }
bool Plater::can_redo() const { return IsShown() && p->is_view3D_shown() && p->undo_redo_stack().has_redo_snapshot(); }
bool Plater::can_reload_from_disk() const { return p->can_reload_from_disk(); }
//BBS
bool Plater::can_fillcolor() const { return p->can_fillcolor(); }
bool Plater::has_assmeble_view() const { return p->has_assemble_view(); }
bool Plater::can_replace_with_stl() const { return p->can_replace_with_stl(); }
bool Plater::can_mirror() const { return p->can_mirror(); }
bool Plater::can_split(bool to_objects) const { return p->can_split(to_objects); }
#if ENABLE_ENHANCED_PRINT_VOLUME_FIT
bool Plater::can_scale_to_print_volume() const { return p->can_scale_to_print_volume(); }
#endif // ENABLE_ENHANCED_PRINT_VOLUME_FIT

const UndoRedo::Stack& Plater::undo_redo_stack_main() const { return p->undo_redo_stack_main(); }
void Plater::clear_undo_redo_stack_main() { p->undo_redo_stack_main().clear(); }
void Plater::enter_gizmos_stack() { p->enter_gizmos_stack(); }
bool Plater::leave_gizmos_stack() { return p->leave_gizmos_stack(); } // BBS: return false if not changed
bool Plater::inside_snapshot_capture() { return p->inside_snapshot_capture(); }

void Plater::toggle_render_statistic_dialog()
{
    p->show_render_statistic_dialog = !p->show_render_statistic_dialog;
}

bool Plater::is_render_statistic_dialog_visible() const
{
    return p->show_render_statistic_dialog;
}

void Plater::toggle_show_wireframe()
{
    p->show_wireframe = !p->show_wireframe;
}

bool Plater::is_show_wireframe() const
{
    return p->show_wireframe;
}

void Plater::enable_wireframe(bool status)
{
    p->wireframe_enabled = status;
}

bool Plater::is_wireframe_enabled() const
{
    return p->wireframe_enabled;
}


/*Plater::TakeSnapshot::TakeSnapshot(Plater *plater, const std::string &snapshot_name)
: TakeSnapshot(plater, from_u8(snapshot_name)) {}
Plater::TakeSnapshot::TakeSnapshot(Plater* plater, const std::string& snapshot_name, UndoRedo::SnapshotType snapshot_type)
: TakeSnapshot(plater, from_u8(snapshot_name), snapshot_type) {}*/


// Wrapper around wxWindow::PopupMenu to suppress error messages popping out while tracking the popup menu.
bool Plater::PopupMenu(wxMenu *menu, const wxPoint& pos)
{
    // Don't want to wake up and trigger reslicing while tracking the pop-up menu.
    SuppressBackgroundProcessingUpdate sbpu;
    // When tracking a pop-up menu, postpone error messages from the slicing result.
    m_tracking_popup_menu = true;
    bool out = wxGetApp().mainframe->PopupMenu(menu, pos);
    m_tracking_popup_menu = false;
    if (! m_tracking_popup_menu_error_message.empty()) {
        // Don't know whether the CallAfter is necessary, but it should not hurt.
        // The menus likely sends out some commands, so we may be safer if the dialog is shown after the menu command is processed.
        wxString message = std::move(m_tracking_popup_menu_error_message);
        wxTheApp->CallAfter([message, this]() { show_error(this, message); });
        m_tracking_popup_menu_error_message.clear();
    }
    return out;
}
void Plater::bring_instance_forward()
{
    p->bring_instance_forward();
}

bool Plater::need_update() const
{
    return p->need_update();
}

void Plater::set_need_update(bool need_update)
{
    p->set_need_update(need_update);
}

int Plater::batch_physical_deletion() const
{
    return p->m_batch_physical_deletion;
}

void Plater::inc_batch_physical_deletion() { ++p->m_batch_physical_deletion; }
void Plater::dec_batch_physical_deletion() { --p->m_batch_physical_deletion; }

// BBS
//BBS: add popup logic for table object
bool Plater::PopupObjectTable(int object_id, int volume_id, const wxPoint& position)
{
    return p->PopupObjectTable(object_id, volume_id, position);
}

bool Plater::PopupObjectTableBySelection()
{
    wxDataViewItem item;
    int obj_idx, vol_idx;
    const wxPoint pos = wxPoint(0, 0);  //Fake position
    wxGetApp().obj_list()->get_selected_item_indexes(obj_idx, vol_idx, item);
    return p->PopupObjectTable(obj_idx, vol_idx, pos);
}

void Plater::update_title_dirty_status()
{
    p->update_title_dirty_status();
}


wxMenu* Plater::plate_menu()            { return p->menus.plate_menu();             }
wxMenu* Plater::object_menu()           { return p->menus.object_menu();            }
wxMenu* Plater::part_menu()             { return p->menus.part_menu();              }
wxMenu* Plater::text_part_menu()        { return p->menus.text_part_menu();         }
wxMenu* Plater::svg_part_menu()         { return p->menus.svg_part_menu();          }
wxMenu* Plater::sla_object_menu()       { return p->menus.sla_object_menu();        }
wxMenu* Plater::default_menu()          { return p->menus.default_menu();           }
wxMenu* Plater::instance_menu()         { return p->menus.instance_menu();          }
wxMenu* Plater::layer_menu()            { return p->menus.layer_menu();             }
wxMenu* Plater::multi_selection_menu()  { return p->menus.multi_selection_menu();   }
wxMenu* Plater::filament_action_menu(int active_filament_menu_id) { return p->menus.filament_action_menu(active_filament_menu_id); }
int     Plater::GetPlateIndexByRightMenuInLeftUI() { return p->m_is_RightClickInLeftUI; }
void    Plater::SetPlateIndexByRightMenuInLeftUI(int index) { p->m_is_RightClickInLeftUI = index; }
SuppressBackgroundProcessingUpdate::SuppressBackgroundProcessingUpdate() :
    m_was_scheduled(wxGetApp().plater()->is_background_process_update_scheduled())
{
    wxGetApp().plater()->suppress_background_process(m_was_scheduled);
}

SuppressBackgroundProcessingUpdate::~SuppressBackgroundProcessingUpdate()
{
    wxGetApp().plater()->schedule_background_process(m_was_scheduled);
}

}}    // namespace Slic3r::GUI

