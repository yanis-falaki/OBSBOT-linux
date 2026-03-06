#include <iostream>
#include <vector>
#include <thread>
#include <codecvt>
#include <cstring>
#include <algorithm>

#include <dev/devs.hpp>

using namespace std;

/// device sn list
std::vector<std::string> kDevs;
std::shared_ptr<Device> dev;

/// call when detect device connected or disconnected
void onDevChanged(std::string dev_sn, bool in_out, void *param)
{
    cout << "Device sn: " << dev_sn << (in_out ? " Connected" : " DisConnected") << endl;

    auto it = std::find_if(kDevs.begin(), kDevs.end(), 
        [&dev_sn](const std::string& item) { return item == dev_sn; });

    if (in_out)
    {
        if (it == kDevs.end())
        {
            kDevs.emplace_back(dev_sn);
        }
    }
    else
    {
        if (it != kDevs.end())
        {
            kDevs.erase(it);
        }
    }

    cout << "Device num: " << kDevs.size() << endl;
}

/// call when camera's status update
void onDevStatusUpdated(void *param, const void *data)
{
    auto *status = static_cast<const Device::CameraStatus *>(data);
    switch (dev->productType())
    {
        /// for tiny series
    case ObsbotProdTiny:
    case ObsbotProdTiny4k:
    case ObsbotProdTiny2:
    case ObsbotProdTiny2Lite:
    case ObsbotProdTinySE:
    case ObsbotProdMeet2:
    case ObsbotProdMeetSE:
    {
        cout << dev->devName().c_str() << " status update:" << endl;
        cout << "zoom value: " << status->tiny.zoom_ratio << endl;
        cout << "ai mode: " << status->tiny.ai_mode << endl;
        break;
    }
        /// for meet series
    case ObsbotProdMeet:
    case ObsbotProdMeet4k:
    {
        cout << dev->devName().c_str() << " status update:" << endl;
        cout << "zoom value: " << status->meet.zoom_ratio << endl;
        cout << "background mode: " << status->meet.bg_mode << endl;
        break;
    }
        /// for tail air
    case ObsbotProdTailAir:
    {
        cout << dev->devName().c_str() << " status update:" << endl;
        cout << "zoom value: " << status->tail_air.digi_zoom_ratio << endl;
        cout << "ai mode: " << status->tail_air.ai_type << endl;
        break;
    }
    default:;
    }
}

/// call when device event notify
void onDevEventNotify(void *param, int event_type, const void *result)
{
    cout << "device event notify, event_type: " << event_type << endl;
}

/// call when file download finished
void onFileDownload(void *param, unsigned int file_type, int result)
{
    cout << "file download callback, file_type: " << file_type << " result: " << result << endl;
}

std::string getProductNameByType(ObsbotProductType type)
{
    std::string productName = "UnKnown";
    switch (type)
    {
    case ObsbotProdTiny:
        productName = "Tiny";
        break;
    case ObsbotProdTiny4k:
        productName = "Tiny4K";
        break;
    case ObsbotProdMeet:
        productName = "Meet";
        break;
    case ObsbotProdMeet4k:
        productName = "Meet4K";
        break;
    case ObsbotProdMe:
        productName = "Me";
        break;
    case ObsbotProdTailAir:
        productName = "TailAir";
        break;
    case ObsbotProdTiny2:
        productName = "Tiny2";
        break;
    case ObsbotProdHDMIBox:
        productName = "HDMIBox";
        break;
    case ObsbotProdTiny2Lite:
        productName = "Tiny2Lite";
        break;
    case ObsbotProdMeet2:
        productName = "Meet2";
        break;
    case ObsbotProdMeetSE:
        productName = "MeetSE";
        break;
    case ObsbotProdTinySE:
        productName = "TinySE";
        break;
    case ObsbotProdButt:
        productName = "Butt";
        break;
    default:
        break;
    }

    return productName;
}

int main(int argc, char **argv)
{
    cout << "Hello World" << endl;
    kDevs.clear();

    /// register device changed callback
    Devices::get().setDevChangedCallback(onDevChanged, nullptr);

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    /// select the first device
    int deviceIndex = 0;
    string cmd;
    cout << "please input command('h' to get command info): " << endl;
    while (cin >> cmd)
    {
        if (cmd == "h")
        {
            cout << "==========================================" << endl;
            cout << "q:             quit!" << endl;
            cout << "p:             printf device info!" << endl;
            cout << "s:             select device!" << endl;
            cout << "1              set status callback!" << endl;
            cout << "2              set event notify callback!" << endl;
            cout << "3              wakeup or sleep!" << endl;
            cout << "4              control the gimbal to move to the specified angle!" << endl;
            cout << "5              control the gimbal to move by the specified speed!" << endl;
            cout << "6              set the boot initial position and zoom ratio and move to the preset position!"
                 << endl;
            cout << "7              set the preset position and move to the preset positions!" << endl;
            cout << "8              set ai mode!" << endl;
            cout << "9              cancel ai mode!" << endl;
            cout << "10             set ai tracking type!" << endl;
            cout << "11             set the absolute zoom level!" << endl;
            cout << "12             set the absolute zoom level and speed!" << endl;
            cout << "13             set fov of the camera!" << endl;
            cout << "14             set media mode!" << endl;
            cout << "15             set hdr!" << endl;
            cout << "16             set face focus!" << endl;
            cout << "17             set the manual focus value!" << endl;
            cout << "18             set the white balance!" << endl;
            cout << "==========================================" << endl;
            cout << "Custom Commands:" << endl;
            cout << "20             set FOV (will prompt for option)!" << endl;
            cout << "21             set camera focus (multiple options)!" << endl;
            cout << "==========================================" << endl;
            cout << "please input command('h' to get command info): ";
            continue;
        }

        if (cmd == "q")
        { exit(0); }

        if (kDevs.empty())
        {
            cout << "No devices connected" << endl;
            cout << "please input command('h' to get command info): ";
            continue;
        }

        /// print device's info
        if (cmd == "p")
        {
            int index = 0;
            cout << "Current connected devices:" << endl;
            auto dev_list = Devices::get().getDevList();
            for (auto &item : dev_list)
            {
                cout << "---------------------------------------------------" << endl;
                cout << "Device SN: " << item->devSn() << endl;
                cout << "  index: " << index++ << endl;
                cout << "  deviceName: " << item->devName().c_str() << endl;
                cout << "  deviceVersion: " << item->devVersion().c_str() << endl;
#ifdef _WIN32
                if (item->devMode() == Device::DevModeUvc)
                {
                    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                    cout << "  videoDevPath: " << converter.to_bytes(item->videoDevPath()).c_str() << endl;
                    cout << "  videoFriendlyName: " << converter.to_bytes(item->videoFriendlyName()).c_str() << endl;
                    cout << "  audioDevPath: " << converter.to_bytes(item->audioDevPath()).c_str() << endl;
                    cout << "  audioFriendlyName: " << converter.to_bytes(item->audioFriendlyName()).c_str() << endl;
                }
#endif
                std::string strProductType = getProductNameByType(item->productType());
                cout << "  product: " << strProductType << endl;
                /// network mode
                if (item->productType() == ObsbotProductType::ObsbotProdTailAir &&
                    item->devMode() == Device::DevModeNet)
                {
                    cout << "  deviceBluetoothMac: " << item->devBleMac().c_str() << endl;
                    cout << "  deviceWifiMode: " << item->devWifiMode().c_str() << endl;
                    if (item->devWifiMode() == "station")
                    {
                        cout << "  deviceWifiSsid: " << item->devWifiSsid().c_str() << endl;
                        cout << "  deviceWiredIp: " << item->devWiredIp().c_str() << endl;
                        cout << "  deviceWirelessIp: " << item->devWirelessIp().c_str() << endl;
                    }
                }
            }
            cout << "please input command('h' to get command info): ";
            continue;
        }

        /// select the first device
        dev = Devices::get().getDevBySn(kDevs[deviceIndex]);

        /// update selected device
        if (cmd == "s")
        {
            cout << "Input the index of device:";
            cin >> deviceIndex;
            if (deviceIndex < 0 || deviceIndex >= kDevs.size())
            {
                cout << "Invalid device index, valid range: 0 ~ " << kDevs.size() - 1 << endl;
                cout << "please input command('h' to get command info): ";
                continue;
            }
            dev = Devices::get().getDevBySn(kDevs[deviceIndex]);
            cout << "select the device: " << dev->devName().c_str() << endl;
            cout << "please input command('h' to get command info): ";
            continue;
        }

        /// control the device to do something
        int cmd_code = atoi(cmd.c_str());
        switch (cmd_code)
        {
            /// set status callback
        case 1:
        {
            dev->setDevStatusCallbackFunc(onDevStatusUpdated, nullptr);
            dev->enableDevStatusCallback(true);
            break;
        }
            /// set event notify callback, only for tail air
        case 2:
        {
            if (dev->productType() == ObsbotProdTailAir)
            {
                dev->setDevEventNotifyCallbackFunc(onDevEventNotify, nullptr);
            }
            break;
        }
            /// wakeup or sleep
        case 3:
        {
            dev->cameraSetDevRunStatusR(Device::DevStatusRun);
            break;
        }
            /// control the gimbal to move to the specified angle, only for tiny2 and tail air
        case 4:
        {
            if (dev->productType() == ObsbotProdTiny2 || dev->productType() == ObsbotProdTailAir)
            {
                dev->aiSetGimbalMotorAngleR(0.0f, -45.0f, 90.0f);
            }
            break;
        }
            /// control the gimbal to move by the specified speed, the gimbal will be stop if the speed is 0
        case 5:
        {
            dev->aiSetGimbalSpeedCtrlR(-45, 60, 60);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            dev->aiSetGimbalSpeedCtrlR(0, 0, 0);
            break;
        }
            /// set the boot initial position and zoom ratio and move to the preset position
        case 6:
        {
            Device::PresetPosInfo BootPosPresetInfo;
            BootPosPresetInfo.id = 0;
            std::string BootPosPresetPosName = "BootPresetInfoZero";
            memcpy(BootPosPresetInfo.name, BootPosPresetPosName.c_str(), BootPosPresetPosName.length());
            BootPosPresetInfo.name_len = BootPosPresetPosName.length();
            BootPosPresetInfo.zoom = 1.4;
            BootPosPresetInfo.yaw = 45.0f;
            BootPosPresetInfo.pitch = 0.0f;
            BootPosPresetInfo.roll = 90.0f;
            BootPosPresetInfo.roi_cx = 2.0;
            BootPosPresetInfo.roi_cy = 2.0;
            BootPosPresetInfo.roi_alpha = 2.0;
            dev->aiSetGimbalBootPosR(BootPosPresetInfo);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            dev->aiTrgGimbalBootPosR();
            break;
        }
            /// set the preset position and move to the preset position
        case 7:
        {
            Device::PresetPosInfo presetInfo;
            presetInfo.id = 0;
            std::string PresetPosName = "PresetInfoZero";
            memcpy(presetInfo.name, PresetPosName.c_str(), PresetPosName.length());
            presetInfo.name_len = PresetPosName.length();
            presetInfo.zoom = 1.6;
            presetInfo.yaw = 25.0f;
            presetInfo.pitch = 45.0f;
            presetInfo.roll = 60.0f;
            presetInfo.roi_cx = 2.0;
            presetInfo.roi_cy = 2.0;
            presetInfo.roi_alpha = 2.0;
            dev->aiAddGimbalPresetR(&presetInfo);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            dev->aiTrgGimbalPresetR(presetInfo.id);
        }
            /// set ai mode
        case 8:
        {
            if (dev->productType() == ObsbotProdTiny || dev->productType() == ObsbotProdTiny4k)
            {
                dev->aiSetTargetSelectR(true);
            }
            else if (dev->productType() == ObsbotProdTiny2)
            {
                dev->cameraSetAiModeU(Device::AiWorkModeHuman, Device::AiSubModeUpperBody);
            }
            else if (dev->productType() == ObsbotProdTailAir)
            {
                dev->aiSetAiTrackModeEnabledR(Device::AiTrackHumanNormal, true);
            }
            break;
        }
            /// cancel ai mode
        case 9:
        {
            if (dev->productType() == ObsbotProdTiny || dev->productType() == ObsbotProdTiny4k)
            {
                dev->aiSetTargetSelectR(false);
            }
            else if (dev->productType() == ObsbotProdTiny2)
            {
                dev->cameraSetAiModeU(Device::AiWorkModeNone);
            }
            else if (dev->productType() == ObsbotProdTailAir)
            {
                int ai_type = dev->cameraStatus().tail_air.ai_type;
                if (ai_type == 5)
                { dev->aiSetAiTrackModeEnabledR(Device::AiTrackGroup, false); }
                else
                { dev->aiSetAiTrackModeEnabledR(Device::AiTrackNormal, false); }
            }
            break;
        }
            /// set ai tracking type
        case 10:
        {
            dev->aiSetTrackingModeR(Device::AiVTrackStandard);
            break;
        }
            /// set the absolute zoom level
        case 11:
        {
            dev->cameraSetZoomAbsoluteR(1.5);
            break;
        }
            /// set the absolute zoom level and speed
        case 12:
        {
            dev->cameraSetZoomWithSpeedAbsoluteR(150, 6);
            break;
        }
            /// set fov of the camera
        case 13:
        {
            dev->cameraSetFovU(Device::FovType86);
            break;
        }
            /// set media mode, only for meet and meet4K
        case 14:
        {
            if (dev->productType() == ObsbotProdMeet || dev->productType() == ObsbotProdMeet4k)
            {
                dev->cameraSetMediaModeU(Device::MediaModeBackground);
                dev->cameraSetBgModeU(Device::MediaBgModeReplace);
            }
            break;
        }
            /// set hdr
        case 15:
        {
            dev->cameraSetWdrR(Device::DevWdrModeDol2TO1);
            break;
        }
            /// set face focus
        case 16:
        {
            dev->cameraSetFaceFocusR(true);
            break;
        }
            /// set the manual focus value
        case 17:
        {
            dev->cameraSetFocusAbsolute(50, false);
            break;
        }
            /// set the white balance
        case 18:
        {
            dev->cameraSetWhiteBalanceR(Device::DevWhiteBalanceAuto, 100);
            break;
        }

        // custom commands
        case 20:
        {
            int fov_option;
            cout << "Select FOV option (1: 86°, 2: 78°, 3: 65°): ";
            cin >> fov_option;
            
            switch(fov_option)
            {
                case 1:
                    dev->cameraSetFovU(Device::FovType86);
                    cout << "FOV set to 86°" << endl;
                    break;
                case 2:
                    dev->cameraSetFovU(Device::FovType78);
                    cout << "FOV set to 78°" << endl;
                    break;
                case 3:
                    dev->cameraSetFovU(Device::FovType65);
                    cout << "FOV set to 65°" << endl;
                    break;
                default:
                    cout << "Invalid option. Please select 1, 2, or 3." << endl;
                    break;
            }
            break;
        }

        // focus control
        case 21:
        {
            int focus_option;
            cout << "Select focus option:" << endl;
            cout << "1: Auto Focus" << endl;
            cout << "2: Manual Focus (custom value)" << endl;
            cout << "3: Face Focus ON" << endl;
            cout << "4: Face Focus OFF" << endl;
            cout << "5: Focus Near (close objects)" << endl;
            cout << "6: Focus Far (distant objects)" << endl;
            cout << "Enter option (1-6): ";
            cin >> focus_option;
            
            switch(focus_option)
            {
                case 1:
                    // Auto focus - assuming there's an auto focus method
                    dev->cameraSetFocusAbsolute(0, true); // true typically enables auto focus
                    cout << "Auto focus enabled" << endl;
                    break;
                case 2:
                {
                    int focus_value;
                    cout << "Enter manual focus value (0-100): ";
                    cin >> focus_value;
                    if (focus_value >= 0 && focus_value <= 100) {
                        dev->cameraSetFocusAbsolute(focus_value, false);
                        cout << "Manual focus set to " << focus_value << endl;
                    } else {
                        cout << "Invalid focus value. Please enter 0-100." << endl;
                    }
                    break;
                }
                case 3:
                    dev->cameraSetFaceFocusR(true);
                    cout << "Face focus enabled" << endl;
                    break;
                case 4:
                    dev->cameraSetFaceFocusR(false);
                    cout << "Face focus disabled" << endl;
                    break;
                case 5:
                    dev->cameraSetFocusAbsolute(20, false); // Near focus (lower values)
                    cout << "Focus set to near (close objects)" << endl;
                    break;
                case 6:
                    dev->cameraSetFocusAbsolute(80, false); // Far focus (higher values)
                    cout << "Focus set to far (distant objects)" << endl;
                    break;
                default:
                    cout << "Invalid option. Please select 1-6." << endl;
                    break;
            }
            break;
        }

        case 22:
        {
            dev->cameraSetWdrR(Device::DevWdrModeNone);
            break;
        }

        case 23:
        {
            dev->cameraSetWdrR(Device::DevWhiteBalanceCloudy);
            break;
        }

        default:;
            cout << "unknown command, please input 'h' to get command info" << endl;
        }
        cout << "please input command('h' to get command info): ";
    }
    return 0;
}
