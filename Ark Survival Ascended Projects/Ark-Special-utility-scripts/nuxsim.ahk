/* 
 * Do not distribute
 *
 * General info:
 * F1: Start
 * F4: Reload script
 * 
 * Start at the screen before server list or server list (before clicking a server)
 *
 * nux <nux@nux.net>
*/

#Requires AutoHotkey >=2.0
#SingleInstance Force
SetWorkingDir(A_ScriptDir)

global settingsFile := A_ScriptDir "\.nuxsim" A_ComputerName ".ini"
global scriptSettings
 
uxtheme := DllCall("GetModuleHandle", "str", "uxtheme", "ptr")
SetPreferredAppMode := DllCall("GetProcAddress", "ptr", uxtheme, "ptr", 135, "ptr")
FlushMenuThemes := DllCall("GetProcAddress", "ptr", uxtheme, "ptr", 136, "ptr")
DllCall(SetPreferredAppMode, "int", 1)	; Dark
DllCall(FlushMenuThemes)
 
scriptSettings_default := []
scriptSettings_default.options := {
    server: 0,
}

scriptSettings := FileExist(settingsFile) ? ReadINI(settingsFile, scriptSettings_default) : scriptSettings_default

global click_locations := Map(
    "2560x1440", Map(
        "serverlist_official_text", {
            x: 1030,
            y: 270,
        },
        "serverlist_filter", {
            x: 2140,
            y: 260,
        },
        "serverlist_join", {
            x: 2230,
            y: 1260,
        },
        "serverlist_top_item", {
            x: 1200,
            y: 435,
        },
        "serverlist_join", {
            x: 2260,
            y: 1255,
        },
        "serverlist_mod_join", { ; 0617
            x: 650,
            y: 1245,
        },
        "serverlist_refresh", {
            x: 1240,
            y: 1250,
        },
        "server_join_failed", { ; Color:	C1F5FF (Red=C1 Green=F5 Blue=FF) color:=SubStr(pixelcolor,3,4) while (color != "FFF5") {
            x: 1340,
            y: 490,
        },
        "server_join_failed2", {
            x: 1445,
            y: 490,
        },
        "server_join_failed3", {
            x: 1100,
            y: 490,
        },
        "server_join_failed_bg", { ; background of server join failed message. not same color as text
            x: 1330,
            y: 490,
        },
        "cancel_auto_rejoin", {
            x: 1415,
            y: 970,
        },
        "other_error_ok", {
            x: 1270,
            y: 880,
        },
        "serverlist_back", {
            x: 230,
            y: 1175,
        },
        "homescreen_error1", {
            x: 1305,
            y: 475,
        },
        "homescreen_error2", {
            x: 1115,
            y: 500,
        },
        "homescreen_error3", {
            x: 1308,
            y: 534,
        },
        "homescreen_error_bg1", {
            x: 1280,
            y: 520,
        },
        "homescreen_error_ok", {
            x: 1270,
            y: 975,
        },
        "homescreen_start", {
            x: 1280,
            y: 1155,
        },
        "pre_list_join", {
            x: 906,
            y: 725,
        },
        "serverlist_official_text1", {
            x: 1030,
            y: 270,
        },
        "serverlist_official_text2", {
            x: 906,
            y: 267,
        },
        "serverlist_official_text3", {
            x: 960,
            y: 305,
        },
        "serverlist_official_text_bg1", {
            x: 967,
            y: 287,
        },
        "other_error_red_text1", {
            x: 1417,
            y: 513,
        },
        "other_error_red_text2", {
            x: 1197,
            y: 513,
        },
        "other_error_red_text3", {
            x: 1307,
            y: 520,
        },
        "other_error_red_bg1", {
            x: 1293,
            y: 547,
        },
    ),
    /*
        "", {
            x: 
        },
    */
    "3440x1440", Map(
        "serverlist_official_text", {
            x: 1470,
            y: 270,
        },
        "serverlist_filter", {
            x: 2595,
            y: 260,
        },
        "serverlist_join", {
            x: 2725,
            y: 1255,
        },
        "serverlist_top_item", {
            x: 1570,
            y: 435,
        },
        "serverlist_join", {
            x: 2660,
            y: 1255,
        },
        "serverlist_refresh", {
            x: 1675,
            y: 1250,
        },
        "serverlist_mod_join", { ; 0617
            x: 1090,
            y: 1250,
        },
        "server_join_failed", {
            x: 1900,
            y: 490,
        },
        "server_join_failed2", {
            x: 1755,
            y: 490,
        },
        "server_join_failed3", {
            x: 1540,
            y: 490,
        },
        "server_join_failed_bg", {
            x: 1765,
            y: 490,
        },
        "cancel_auto_rejoin", {
            x: 1870,
            y: 970,
        },
        "other_error_red_text1", {
            x: 1651,
            y: 513,
        },
        "other_error_red_text2", {
            x: 1710,
            y: 525,
        },
        "other_error_red_text3", {
            x: 1860,
            y: 520,
        },
        "other_error_red_bg1", {
            x: 1735,
            y: 510,
        },
        "other_error_ok", {
            x: 1270,
            y: 880,
        },
        "serverlist_back", {
            x: 660,
            y: 1180,
        },
        "homescreen_error1", {
            x: 1555,
            y: 485,
        },
        "homescreen_error2", {
            x: 1800,
            y: 550,
        },
        "homescreen_error3", {
            x: 1695,
            y: 540,
        },
        "homescreen_error_bg1", {
            x: 1820,
            y: 550,
        },
        "homescreen_error_ok", {
            x: 1640,
            y: 975,
        },
        "homescreen_start", {
            x: 1710,
            y: 1140,
        },
        "pre_list_join", {
            x: 1130,
            y: 750,
        },
        "serverlist_official_text1", {
            x: 1470,
            y: 265,
        },
        "serverlist_official_text2", {
            x: 1335,
            y: 265,
        },
        "serverlist_official_text3", {
            x: 1400,
            y: 305,
        },
        "serverlist_official_text_bg1", {
            x: 1400,
            y: 290,
        },
    ),
    "1920x1080", Map(
        "serverlist_refresh", {
            x: 930,
            y: 940,
        },
        "server_join_failed2", {
            x: 870,
            y: 365,
        },
        ; todo: add below to 3440 and translate to 2560
        "serverlist_official_text1", {
            x: 772,
            y: 202,
        },
        "serverlist_official_text2", {
            x: 675,
            y: 200,
        },
        "serverlist_official_text3", {
            x: 720,
            y: 230,
        },
        "serverlist_official_text_bg1", {
            x: 725,
            y: 215,
        },
        "homescreen_error1", {
            x: 1015,
            y: 410,
        },
        "homescreen_error2", {
            x: 895,
            y: 425,
        },
        "homescreen_error3", {
            x: 840,
            y: 370,
        },
        "homescreen_error_bg1", {
            x: 820,
            y: 370,
        },
        "other_error_red_text1", {
            x: 1063,
            y: 385,
        },
        "other_error_red_text2", {
            x: 898,
            y: 385,
        },
        "other_error_red_text3", {
            x: 980,
            y: 390,
        },
        "other_error_red_bg1", {
            x: 970,
            y: 410,
        },
    )
)

global is_active := False

get_gui()
toggle_gui()

f1:: {
    global is_active := !is_active

    myGui := get_gui()

    if (myGui.server_num.Value = 0) {
        MsgBox "Please set server!"
        return
    }

    if (is_active) {
        SetTimer(repeat1, 1000)
        myGui.server_num.Enabled := False
        timer_status("start")
    } else {
        SetTimer repeat1, 0
        myGui.server_num.Enabled := True
        timer_status("idle")
        ; Rough stop all
        Reload
    }
}

f4:: {
    Reload
}

repeat1() {
    attempt_join()
}

global join_inactive_success := False

attempt_join() {
    global join_inactive_success

    serverlist_top_item := get_loc("serverlist_top_item")
    cancel_auto_rejoin := get_loc("cancel_auto_rejoin")
    other_error_ok := get_loc("other_error_ok")
    serverlist_back := get_loc("serverlist_back")
    homescreen_error_ok := get_loc("homescreen_error_ok")
    homescreen_start := get_loc("homescreen_start")
    pre_list_join := get_loc("pre_list_join")
    serverlist_join := get_loc("serverlist_join")
    serverlist_mod_join := get_loc("serverlist_mod_join")
    serverlist_refresh := get_loc("serverlist_refresh")

    ark_activate()

    myGui := get_gui()

    if (home_screen_error() = true) {
        timer_status("home screen error")
        ; accept error
        ControlClick "x" homescreen_error_ok.x " y" homescreen_error_ok.y, "ahk_exe ArkAscended.exe"

        sleep 1000

        ControlClick "x" homescreen_start.x " y" homescreen_start.y, "ahk_exe ArkAscended.exe"

        sleep 1000
    }

    if (can_see_server_list()) {
        sleep 125
        timer_status("filter")
        filter_servers(myGui.server_num.Value)
        sleep 500

        ark_activate()

        if (!join_inactive_success) {
            join_inactive := PixelGetColor(serverlist_join.x, serverlist_join.y)
        } else {
            join_inactive := join_inactive_success
        }

        timer_status("click server")
        num := 0
        diff := 0
        while (diff < 80) {
            ControlClick "x" serverlist_top_item.x " y" serverlist_top_item.y, "ahk_exe ArkAscended.exe"
            sleep 500

            if (num > 5) {
                timer_status("refresh server")

                ControlClick "x" serverlist_refresh.x " y" serverlist_refresh.y, "ahk_exe ArkAscended.exe"
                sleep 1000

                ControlClick "x" serverlist_top_item.x " y" serverlist_top_item.y, "ahk_exe ArkAscended.exe"
                sleep 500
            } else {
                num += 1
            }

            ark_activate()            
            join_current := PixelGetColor(serverlist_join.x, serverlist_join.y)
            diff := color_diff(join_inactive, join_current)
            ;ToolTip diff
            ;sleep 500
        }

        join_active := PixelGetColor(serverlist_join.x, serverlist_join.y)

        timer_status("click join")
        join_visible := PixelGetColor(serverlist_join.x, serverlist_join.y)

        sleep 250

        num := 0
        diff := 0
        while (diff < 20) {
            ControlClick "x" serverlist_join.x " y" serverlist_join.y, "ahk_exe ArkAscended.exe"
            
            sleep 500
            num += 1

            ark_activate()
            join_hidden := PixelGetColor(serverlist_join.x, serverlist_join.y)
            diff := color_diff(join_visible, join_hidden)

            if (num > 10) {
                ; try clicking top item again
                ControlClick "x" serverlist_top_item.x " y" serverlist_top_item.y, "ahk_exe ArkAscended.exe"
                sleep 500

                num := 0

                join_hidden := PixelGetColor(serverlist_join.x, serverlist_join.y)
                diff := color_diff(join_visible, join_hidden)

                if (diff < 20) {
                    join_active := "0x271302"
                }
            }
        }

        sleep 250
        timer_status("mod wait")
        ; wait for mod button to show
        diff := 100
        ark_activate()

        num := 0
        ; have gotten stuck here waiting for mods from server
        while (diff > 50) {
            num += 1

            mod_current := PixelGetColor(serverlist_mod_join.x, serverlist_mod_join.y)
            sleep 250

            diff := color_diff(join_active, mod_current)

            ; have gotten stuck where join didnt get clicked on 3440
            ; try clicking join again if we have waited between 5 and 60 seconds
            if (num > 20 AND num < 240) {
                join_before_click := PixelGetColor(serverlist_join.x, serverlist_join.y)
                ControlClick "x" serverlist_top_item.x " y" serverlist_top_item.y, "ahk_exe ArkAscended.exe"
                sleep 125
                join_after_click := PixelGetColor(serverlist_join.x, serverlist_join.y)

                if (color_diff(join_before_click, join_after_click) > 20) {
                    join_active := PixelGetColor(serverlist_join.x, serverlist_join.y)
                    sleep 125
                    ControlClick "x" serverlist_join.x " y" serverlist_join.y, "ahk_exe ArkAscended.exe"
                    sleep 125
                }
                
            }
        }

        sleep 250
        timer_status("mod click")

        ark_activate()
        ; click mod join
        diff := 25
        while (diff > 20) {
            sleep 125
            ControlClick "x" serverlist_mod_join.x " y" serverlist_mod_join.y, "ahk_exe ArkAscended.exe"
            sleep 500

            join_hidden := PixelGetColor(serverlist_join.x, serverlist_join.y)
            diff := color_diff(join_inactive, join_hidden)
        }

        ark_activate()
        sleep 250
        timer_status("wait conn failed")

        wait_connection_failed()
        sleep 250
        ark_activate()
        if (has_other_error() = true) {
            ; other error message in middle
            ControlClick "x" other_error_ok.x " y" other_error_ok.y, "ahk_exe ArkAscended.exe"
            sleep 500
        }

        ; deal with home screen error on beginning of loop
        if (home_screen_error() = true) {
            return
        }

        timer_status("cancel rejoin")
        ControlClick "x" cancel_auto_rejoin.x " y" cancel_auto_rejoin.y, "ahk_exe ArkAscended.exe"        ; click cancel auto rejoin
        sleep 250
        

        timer_status("back out")
        ControlClick "x" serverlist_back.x " y" serverlist_back.y, "ahk_exe ArkAscended.exe"        ; click back out of server list

        if (!join_inactive_success) {
            join_inactive_success := join_inactive
        }

        if (home_screen_error() = true) {
            ; accept error
            ControlClick "x" homescreen_error_ok.x " y" homescreen_error_ok.y, "ahk_exe ArkAscended.exe"

            sleep 1000

            ControlClick "x" homescreen_start.x " y" homescreen_start.y, "ahk_exe ArkAscended.exe"
            sleep 500
        }
    } else {
        timer_status("join servers")
        ControlClick "x" pre_list_join.x " y" pre_list_join.y, "ahk_exe ArkAscended.exe" ; join game
    }

    sleep 1000
}

get_loc(location) {
    global click_locations

    res := A_ScreenWidth "x" A_ScreenHeight

    if not click_locations.has(res) {
        MsgBox("Resolution " res " not found...")
        sleep 1500
        Reload
    }

    if not click_locations[res].has(location) {
        ; attempt auto translation of 2560->1920
        if (res = "1920x1080") {
            if (click_locations["2560x1440"].has(location)) {
                loc := {
                    x: trans_x(click_locations["2560x1440"][location].x),
                    y: trans_x(click_locations["2560x1440"][location].y),
                }

                return loc
            }
        }
        MsgBox("Location " location " " res " not found...")
        sleep 1500
        Reload
    }

    return click_locations[res][location]
}

color_diff(color1, color2:=0){
	r1 := (color1 & 0xFF0000) >> 16
	g1 := (color1 & 0xFF00) >> 8
	b1 := (color1 & 0xFF)
	r2 := (color2 & 0xFF0000) >> 16
	g2 := (color2 & 0xFF00) >> 8
	b2 := (color2 & 0xFF)
	dr := Abs(r1-r2)
	dg := Abs(g1-g2)
	db := Abs(b1-b2)
	return (dr > dg) && (dr > db) ? (dr) : ((dg > db) ? (dg) : (db))
}

wait_connection_failed() {
    ; new method for fiding window
    ; find window when diff1 < 20 and diff2 < 20 and bg_diff > 20
    while (!has_conn_failed_error()) {
        sleep 500

        if (has_other_error() = true) {
            return
        }

        if (home_screen_error() = true) {
            return
        }
    }

    return
}

has_conn_failed_error() {
    server_join_failed := get_loc("server_join_failed")
    server_join_failed2 := get_loc("server_join_failed2")
    server_join_failed3 := get_loc("server_join_failed3")
    server_join_failed_bg := get_loc("server_join_failed_bg")

    loc1 := PixelGetColor(server_join_failed.x, server_join_failed.y)
    loc2 := PixelGetColor(server_join_failed2.x, server_join_failed2.y)
    loc3 := PixelGetColor(server_join_failed3.x, server_join_failed3.y)

    bg_loc1 := PixelGetColor(server_join_failed_bg.x, server_join_failed_bg.y)

    diff1 := color_diff(loc1, loc2)
    diff2 := color_diff(loc1, loc3)
    bg_diff := color_diff(loc1, bg_loc1)

    ; find window when diff1 < 20 and diff2 < 20 and bg_diff > 20
    if (diff1 < 20 and diff2 < 20 and bg_diff > 20) {
        ; verify blue hue is FF
        if (SubStr(loc1,7,2) = "FF") {
            ;ToolTip

            return true
        }
        
    }

    return false
    
}

is_join_active() {
    MouseMove 2525, 1260
    sleep 125
    join_col1 := PixelGetColor(2650, 1255)
    sleep 25

    MouseMove 2600, 1260
    sleep 125
    join_col2 := PixelGetColor(2650, 1255)

    sleep 500
    ToolTip(color_diff(join_col1, join_col2))
}

has_other_error() {
    other_error_red_text1 := get_loc("other_error_red_text1")
    other_error_red_text2 := get_loc("other_error_red_text2")
    other_error_red_text3 := get_loc("other_error_red_text3")
    other_error_red_bg1 := get_loc("other_error_red_bg1")

    loc1 := PixelGetColor(other_error_red_text1.x, other_error_red_text1.y)
    loc2 := PixelGetColor(other_error_red_text2.x, other_error_red_text2.y)
    loc3 := PixelGetColor(other_error_red_text3.x, other_error_red_text3.y)

    bg_loc1 := PixelGetColor(other_error_red_bg1.x, other_error_red_bg1.y)

    diff1 := color_diff(loc1, loc2)
    diff2 := color_diff(loc1, loc3)
    bg_diff := color_diff(loc1, bg_loc1)

    if (diff1 < 20 and diff2 < 20 and bg_diff > 20) {
        return true        
    }
    return false
}

can_see_server_list() {
    serverlist_official_text1 := get_loc("serverlist_official_text1")
    serverlist_official_text2 := get_loc("serverlist_official_text2")
    serverlist_official_text3 := get_loc("serverlist_official_text3")
    serverlist_official_text_bg1 := get_loc("serverlist_official_text_bg1")

    loc1 := PixelGetColor(serverlist_official_text1.x, serverlist_official_text1.y)
    loc2 := PixelGetColor(serverlist_official_text2.x, serverlist_official_text2.y)
    loc3 := PixelGetColor(serverlist_official_text3.x, serverlist_official_text3.y)

    bg_loc1 := PixelGetColor(serverlist_official_text_bg1.x, serverlist_official_text_bg1.y)

    diff1 := color_diff(loc1, loc2)
    diff2 := color_diff(loc1, loc3)
    bg_diff := color_diff(loc1, bg_loc1)

    if (diff1 < 20 and diff2 < 20 and bg_diff > 20) {
        if (SubStr(loc1,7,2) = "FF") {
            return true
        }
    }

    return false
}

home_screen_error() {
    homescreen_error1 := get_loc("homescreen_error1")
    homescreen_error2 := get_loc("homescreen_error2")
    homescreen_error3 := get_loc("homescreen_error3")
    homescreen_error_bg1 := get_loc("homescreen_error_bg1")

    loc1 := PixelGetColor(homescreen_error1.x, homescreen_error1.y)
    loc2 := PixelGetColor(homescreen_error2.x, homescreen_error2.y)
    loc3 := PixelGetColor(homescreen_error3.x, homescreen_error3.y)

    bg_loc1 := PixelGetColor(homescreen_error_bg1.x, homescreen_error_bg1.y)

    diff1 := color_diff(loc1, loc2)
    diff2 := color_diff(loc1, loc3)
    bg_diff := color_diff(loc1, bg_loc1)

    ;timer_status("loc1: " loc1 " loc2: " loc2 " " loc3 " diffs: " diff1 " " diff2 " " bg_diff)
    ;ToolTip "loc1: " loc1 " loc2: " loc2 " " loc3 " diffs: " diff1 " " diff2 " " bg_diff " " bg_loc1

    ; find window when diff1 < 20 and diff2 < 20 and bg_diff > 20
    if (diff1 < 20 and diff2 < 20 and bg_diff > 20) {
        ; verify blue hue is FF
        if (SubStr(loc1,7,2) = "FF") {
            ;ToolTip

            return true
        }
        
    }

    return false
}

filter_servers(text) {
    serverlist_filter := get_loc("serverlist_filter")

    ;MouseMove serverlist_filter.x, serverlist_filter.y
    ;sleep 25
    ControlClick "x" serverlist_filter.x " y" serverlist_filter.y, "ahk_exe ArkAscended.exe"
    sleep 25
    ;ControlSend "^a",,"ahk_exe ArkAscended.exe"
    ;Send("^a")
    ;sleep 25
    ;Send(text)
    ControlSend text,,"ahk_exe ArkAscended.exe"
}

ark_activate() {
    if WinExist("ahk_exe ArkAscended.exe") {
        WinActivate
    }
}

timer_status(status) {
    myGui := get_gui()

    myGui.scriptStatus.Value := status
}

trans_x(n) {
    return n * (A_ScreenWidth / 2560)	
}

trans_y(n) {
    return n * (A_ScreenHeight / 1440)
}



get_gui() {
    global myGui, guiShown, guiWindow

    if (!isSet(myGui)) {
        myGui := False
    }

    if (!isSet(guiShown)) {
        guiShown := False
    }

    if (!isSet(guiWindow)) {
        guiWindow := { Width: 165, Height: 100 }
    }

    if (!myGui) {
        guiSettings()
    }

    return myGui
}

get_gui_x(width:=250) {
    mid_x := A_ScreenWidth // 2

    half_width := Round(width/2)

    return mid_x - half_width
}

get_gui_y(height:=250) {
    return 10
}

toggle_gui() {
    global myGui,guiShown,guiWindow

    guiShown := !guiShown

    if (guiShown) {
        myGui.Show(" w" guiWindow.Width " h" guiWindow.Height " x" get_gui_x() " y" get_gui_y() " NoActivate")
    } else {
        myGui.Hide()
    }
}

hide_gui() {
    global myGui,guiShown,guiWindow

    guiShown := False

    myGui.Hide()
}

show_gui() {
    global myGui,guiShown,guiWindow

    guiShown := True

    myGui.Show(" w" guiWindow.Width " h" guiWindow.Height " x" 240 " y" 60 " NoActivate")
}

guiSettings(){
    global myGui, guiWindow

    myGui := Gui()
    myGui.Opt("-0x30000 AlwaysOnTop")

    ; Changing the title bar to dark
    attr := VerCompare(A_OSVersion, "10.0.18985") >= 0 ? 20 : VerCompare(A_OSVersion, "10.0.17763") >= 0 ? 19 : ""
    DllCall("dwmapi\DwmSetWindowAttribute", "ptr", MyGui.hwnd, "int", attr, "int*", true, "int", 4) 

    ; Changing the Icon of the window
    hIcon32 := LoadPicture("wmploc", "w16 h-1 Icon18", &IMAGE_ICON := 1)
    myGui.Opt("+LastFound")	; Set our GUI as LastFound window ( affects next two lines )
    ErrorLevel := SendMessage((WM_SETICON := 0x80), 0, hIcon32)	; Set the Titlebar Icon
    ErrorLevel := SendMessage((WM_SETICON := 0x80), 1, hIcon32)	; Set the Alt-Tab icon

    myGui.Title := "nux sim"
    myGui.iniFile := settingsFile

    ; Exit script when GUI is closed
    myGui.OnEvent("Close", (*) => ExitApp(0))

    myGui.AddText("XM yp+5 w40", "Status")
    myGui.scriptStatus := myGui.AddEdit("v_scriptStatus yp w100 -E0x200 hp+5 Disabled Center", "idle")

    myGui.AddText("XM yp+25 w100", "Start/Stop")
    myGui.AddEdit("yp x+10 -E0x200 hp+5 w20 Disabled", "F1")

    myGui.AddText("XM yp+20 w100", "Reload script")
    myGui.AddEdit("yp x+10 -E0x200 hp+5 w20 Disabled", "F4")

    myGui.AddText("XM yp+25 w100", "Server")
    myGui.server_num := myGui.AddEdit("vserver yp w35 h20 Number Center", "0")

    loadSettings(scriptSettings)
    Return

    loadSettings(scriptSettings1){
        oSection := Map()
        For Section, SubObject in scriptSettings1.OwnProps() {
            For Key, Value in SubObject.OwnProps() {
                oSection[StrLower(Key)] := Section
            }
        }
        
        For Hwnd, GuiCtrlObj in MyGui{            
            if (GuiCtrlObj.Type="Button"){
                DllCall("uxtheme\SetWindowTheme", "ptr", GuiCtrlObj.hwnd, "str", "DarkMode_Explorer", "ptr", 0)
            }

            key := GuiCtrlObj.HasOwnProp("key") ? GuiCtrlObj.key : GuiCtrlObj.Name
            if (GuiCtrlObj.Type = "Radio" and key = "" and GuiCtrlObj_Prev.Type= "Radio") {
                key := GuiCtrlObj_Prev.HasOwnProp("key") ? GuiCtrlObj_Prev.key : GuiCtrlObj_Prev.Name
            }
    
            Section := GuiCtrlObj.HasOwnProp("section") ? GuiCtrlObj.section : oSection.Has(StrLower(key)) ? oSection[StrLower(Key)] : ""

            ;MsgBox(Section " " key)
    
            if (Section !="" and key != ""){
                if scriptSettings1.HasOwnProp(Section) and scriptSettings1.%Section%.HasOwnProp(Key){
                    KeyValue := scriptSettings1.%Section%.%Key%
                }
    
                GuiCtrlObj.Section := Section
                GuiCtrlObj.Key := key
    
                if (GuiCtrlObj.Type ~= "^(?i:ComboBox|DDL|Edit)$"){
                    GuiCtrlObj.Text := KeyValue
                    GuiCtrlObj.OnEvent("Change", gApply.Bind(""))
                    ;GuiCtrlObj.OnEvent("Change", gEdit)
                }
                else if (GuiCtrlObj.Type ~= "^(?i:DateTime|Hotkey|MonthCal|Slider)$") {
                    GuiCtrlObj.Value := KeyValue
                    GuiCtrlObj.OnEvent("Change", gEdit)
                }
                else if (GuiCtrlObj.Type = "Checkbox") {
                    if (KeyValue = "true" or KeyValue = 1) {
                        GuiCtrlObj.Value := 1
                    }
                    ;GuiCtrlObj.Value := KeyValue = "true" ? 1 : 0
                    GuiCtrlObj.OnEvent("Click", gEdit)
                }
                else if (GuiCtrlObj.Type = "Radio") {
                    GuiCtrlObj.Value := KeyValue = GuiCtrlObj.Text ? 1 : 0
                    GuiCtrlObj.OnEvent("Click", gEdit)
                }
                else if (GuiCtrlObj.Type = "ListBox"){
                    GuiCtrlObj.Delete()
                    GuiCtrlObj.Add(StrSplit(KeyValue, "|"))
                    GuiCtrlObj.OnEvent("Change", gEdit)
                }
            }
            GuiCtrlObj_Prev := GuiCtrlObj
        }
        return
    }

    gApply(Mode, GuiCtrlObj, Info, *){ 
        For Hwnd, GuiCtrlObj in GuiCtrlObj.Gui {

            if (GuiCtrlObj.HasOwnProp("Section")){
                KeyValue := GuiCtrlObj.Text
                if (GuiCtrlObj.Type ~= "^(?i:DateTime|Hotkey|MonthCal|Slider)$") {
                    KeyValue := GuiCtrlObj.Value
                }
                if (GuiCtrlObj.Type = "Radio"){
                    if GuiCtrlObj.Value = 0 {
                        continue
                    }
                }
                if (GuiCtrlObj.Type = "Checkbox"){
                    KeyValue := GuiCtrlObj.Value = 1 ? "true" : "false"
                }

                if (GuiCtrlObj.Type = "Listbox"){
                    KeyValue := ""
                    For Item in ControlGetItems(GuiCtrlObj) {
                       KeyValue .= (KeyValue ? "|" : "") Item
                    }
                }

                IniWrite(KeyValue , GuiCtrlObj.Gui.iniFile, GuiCtrlObj.Section, GuiCtrlObj.Key)
                Section := GuiCtrlObj.Section
                Key := GuiCtrlObj.Key
                if !scriptSettings.HasOwnProp(Section) {
                    scriptSettings.%Section% := []
                }
                scriptSettings.%Section%.%Key% := KeyValue
            }
        }
        
        Return
    }

    gEdit(GuiCtrlObj, Info, *){
        return

    }

    gExit(*){ ; Exit without saving the changes
        myGui.Destroy()
        ExitApp
        return
    }
}


;-------------------------------------------------------------------------------
writeINI(&array2D, INI_File) {	; write 2D-array to INI-file
    ;-------------------------------------------------------------------------------
    for sectionName, entry in array2D {
        pairs := ""
        for key, value in entry
            pairs .= key "=" value "`n"
        IniWrite(pairs, INI_File, sectionName)
    }

    if !InStr(FileGetAttrib(INI_File), "H") {
        FileSetAttrib "+H", INI_File
    }
}

readINI(INI_File, oResult := "") {	; return 2D-array from INI-file
    oResult := IsObject(oResult) ? oResult : []
    oResult.Section := []
    sectionNames := IniRead(INI_File)
    for each, Section in StrSplit(sectionNames, "`n") {
        outputVar_Section := IniRead(INI_File, Section)
        if !oResult.HasOwnProp(Section){
            oResult.%Section% := []
        }
        for each, haystack in StrSplit(outputVar_Section, "`n"){
            RegExMatch(haystack, "(.*?)=(.*)", &match)
            arrayProperty := match[1]
            oResult.%Section%.%arrayProperty% := match[2]
        }
    }
    return oResult
}
