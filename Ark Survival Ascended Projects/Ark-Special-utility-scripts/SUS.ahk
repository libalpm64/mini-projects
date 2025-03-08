#NoEnv
#SingleInstance Force
SetWorkingDir %A_ScriptDir%
CoordMode, Pixel, Screen
CoordMode, Mouse, Client

GFNActive := 0
ArkActive := 0
MammothActive := False
AntiAfkActive := 0
MagicTActive := 0
MagicTarget := 1

Gui, +Resize +MinSize300x200
Gui, Add, Tab3, vMyTabControl gTabControl w300 h200, Main|Settings
Gui, Tab, Main
Gui, Font, s9 bold, Segoe UI
Gui, Add, Text, x10 y30 w280 Center, SUS v1.0 1920x1080
Gui, font, s8 norm, Segoe UI
itemWidth := 135
itemHeight := 25
xMargin := 15
yMargin := 55

Loop, 6 {
    row := Ceil(A_Index / 2) - 1
    col := Mod(A_Index, 2)
    if (col = 0)
        col := 2
    
    x := xMargin + (col - 1) * itemWidth
    y := yMargin + row * itemHeight
    
    text := % (A_Index = 1) ? "GFN Clicker (F1)" 
          : (A_Index = 2) ? "Ark Clicker (F2)"
          : (A_Index = 3) ? "Sim-Bot (F3)"
          : (A_Index = 4) ? "Rhynio Pick Helper (F4)"
          : (A_Index = 5) ? "Mammoth Clicker (F5)"
	  : (A_Index = 6) ? "Reload Script (F7)"
	  : "Reload Script (F7)"
    
    Gui, Add, Button, x%x% y%y% w%itemWidth% h%itemHeight%, %text%
}

Gui, Tab, Settings
Gui, Font, s8, Segoe UI
Gui, Add, Text, x30 y30, Magic Keybind:
Gui, Add, DropDownList, x30 y+5 w100 vMagicTarget, GFN|ArkAscended.exe
Gui, Add, DropDownList, x+5 w40 vMagicKeybind, T|O|F|A|E
Gui, Add, Button, x+5 w50 vMagicKeybindToggle gMagicKeybindToggle, Off
Gui, Add, Text, x30 y+10, Anti-AFK Monitor:
Gui, Add, Button, x+5 w50 vAntiAfkButton gAntiAfkToggle, Off
Gui, Add, Text, x30 y+10, Magic Delay (ms):
Gui, Add, Edit, x+5 w50 vMagicDelay, 333
Gui, Add, Text, x30 y+10, Monitor Number:
Gui, Add, Edit, x+5 w30 vMonitorNumber, 3

Gui, Show, AutoSize Center, My GUI

Menu, Tray, NoStandard
Menu, Tray, Add, Show GUI, ShowGui
Menu, Tray, Add, Exit, ExitHandler
return

GuiClose:
GuiEscape:
    MsgBox, 3, Exit Options, SUS: Do you want to exit the program or run it in the background?`n`nYes = Exit`nNo = Run in Background`nCancel = Do Nothing
    IfMsgBox, Yes
    {
        ExitApp
    }
    IfMsgBox, No
    {
        Gui, Hide
    }
return

ShowGui:
    Gui, Show
return

ExitHandler:
    ExitApp
return

GuiSize:
    if (A_EventInfo == 1)  ; The window has been minimized.
        return
    GuiControl, Move, MyTabControl, w%A_GuiWidth% h%A_GuiHeight%
return

TabControl:
    GuiControlGet, MyTabControl
return

~*F1::
    GFNActive := !GFNActive
    if (GFNActive) {
        SetTimer, GFNClicker, 100
    } else {
        SetTimer, GFNClicker, Off
    }
    UpdateButtonText("GFNButton", GFNActive)
    return

~*F2::
    if (ArkActive) {
        ArkActive := 0
        SetTimer, ArkClicker, Off
    } else {
        ArkActive := 1
        SetTimer, ArkClicker, 100
    }
    UpdateButtonText("ArkButton", ArkActive)
return

; XButton2 - XButton1 To Apply it to the side buttons (Aka forward X2 and backwards X1)
~*XButton2::
    ; Check if the Control key is pressed, if not press it
    if !GetKeyState("Ctrl", "P")
    {
        ControlSend,, {Ctrl Down}, ahk_exe ArkAscended.exe
    }
    ControlClick, x100 y100, ahk_exe ArkAscended.exe,,,, Pos
    Sleep 2000
    ControlSend,, {Ctrl Up}, ahk_exe ArkAscended.exe
return

~*F5::
    if (MammothActive) {
        MammothActive := False
        SetTimer, MammothClicker, Off  ; Stop checking for the color
    } else {
        MammothActive := True
        SetTimer, MammothClicker, 0  ; Start checking for the color as fast as possible
    }
return

AntiAfkToggle:
    if (AntiAfkActive) {
        AntiAfkActive := 0
        SetTimer, AntiAfkMonitor, Off
    } else {
        AntiAfkActive := 1
        SetTimer, AntiAfkMonitor, 1000
    }
    UpdateButtonText("AntiAfkButton", AntiAfkActive)
return

MagicKeybindToggle:
    if (MagicTActive) {
        MagicTActive := 0
        SetTimer, MagicT, Off
    } else {
        MagicTActive := 1
        SetTimer, MagicT, 1000
    }
    UpdateButtonText("MagicKeybindToggle", MagicTActive)
return

UpdateButtonText(ButtonName, Active) {
    GuiControl,, %ButtonName%, % Active ? "On" : "Off"
}

GFNClicker:
    ControlClick, x100 y100, ahk_exe GeForceNOW.exe,,,, Pos
    Sleep 100
Return

ArkClicker:
    ControlClick, x100 y100, ahk_exe ArkAscended.exe,,,, Pos
    Sleep 100
Return

MammothClicker:
   if (!MammothActive) {
        return
    }
    SearchBoxes := []
    ; edit this for you monitor this is for the 1920x1080
    SearchBoxes.Push([-1124, 896, -1075, 939])  ; Box 1
    SearchBoxes.Push([792, 896, 840, 939])  ; Box 2
    SearchBoxes.Push([2712, 896, 2760, 939])  ; Box 3
    TargetColor := 0x00FDFF  ; RGB: Red=00, Green=FD, Blue=FF

    TargetWindowClass := "UnrealWindow"
    TargetExe := "ArkAscended.exe"

    Loop
    {
        WinGet, hwnd, ID, ahk_class %TargetWindowClass%
        ColorFound := False
        
        ; Iterate through each search box
        for Index, Box in SearchBoxes
        {
            ; Unpack the coordinates
            SearchBoxX1 := Box[1]
            SearchBoxY1 := Box[2]
            SearchBoxX2 := Box[3]
            SearchBoxY2 := Box[4]

            PixelSearch, FoundX, FoundY, SearchBoxX1, SearchBoxY1, SearchBoxX2, SearchBoxY2, TargetColor, 0, Fast, RGB
            if !ErrorLevel  
            {
                ColorFound := True
                
                ; Debugging Output - Ommited use this if you're on a different resolution.
                ; MsgBox, Color found in box %Index% at Client X: %FoundX% Y: %FoundY%
                
                ControlClick, x100 y100, ahk_exe ArkAscended.exe,,,, Pos
        	
		; Cooldown because it might ommit color not found.
                sleep 500
            }
        }
        
        ; Debug stuff
        ; if (!ColorFound)
        ; {
        ;     MsgBox, Color not found in any of the boxes
        ; }
        
        ; Pause for 1 millisecond to prevent cpu spikage
        Sleep 1
    }
Return


global winTarget := "ArkAscended"

; GC SIM Start
class ArkWindow
{
    Title := ""
    Hwnd := ""
    Dims := { "X": 0, "Y": 0, "Width": 0, "Height": 0}

    Init() {
        this.Title := ArkAscended
        WinActivate, ArkAscended
        WinGet, hWnd, ID, ArkAscended
        WinGetPos, x, y, w, h, ArkAscended

        this.Hwnd := hWnd
        this.Dims.X := x
        this.Dims.Y := yi
        this.Dims.Width := w
        this.Dims.Height := h
    }

CheckPixelColorGeneric(pixObj, ByRef uiControl, ByRef isMatch) {
    variation := 0  ; Variation value

    PixelGetColor, pixCol, pixObj.x, pixObj.y

    ; Get the target color from pixObj
    targetColor := pixObj.color

    ; Extract the individual components of the target color (R, G, B)
    targetRed := targetColor >> 16 & 0xFF
    targetGreen := targetColor >> 8 & 0xFF
    targetBlue := targetColor & 0xFF

    ; Extract the individual components of the actual pixel color (R, G, B)
    actualRed := pixCol >> 16 & 0xFF
    actualGreen := pixCol >> 8 & 0xFF
    actualBlue := pixCol & 0xFF

    ; Check if the actual pixel color is within the variation range of the target color
    if (Abs(targetRed - actualRed) <= variation && Abs(targetGreen - actualGreen) <= variation && Abs(targetBlue - actualBlue) <= variation) {
        uiControl := true
        isMatch := true
    } else {
        uiControl := false
        isMatch := false
    }
}


    ClickArk(xPix, yPix) {
        ControlClick, x%xPix% y%yPix%, ArkAscended
    }
}

; GC SimBot Start
class SimBot
{
    ArkWindow := {}
    ScreenWidth := 0
    ScreenHeight := 0
    Is1080 := 0
    Is1440 := 0

    Init() {
        this.ArkWindow := new ArkWindow()
        this.ArkWindow.Init()

        this.ScreenWidth := A_ScreenWidth
        this.ScreenHeight := A_ScreenHeight

        this.Is1080 := this.ArkWindow.Dims.Width = 1920 && this.ArkWindow.Dims.Height = 1080 && this.ArkWindow.Dims.Height = this.ScreenHeight
        this.Is1440 := this.ArkWindow.Dims.Width = 2560 && this.ArkWindow.Dims.Height = 1440 && this.ArkWindow.Dims.Height = this.ScreenHeight

        this.InitCoords()
    }

    Start() {
        if (!this.Is1080 && !this.Is1440) {
            msgbox your game resolution must be either 1920x1080 or 2560x1440 and must match your screen resolution

            return
        }

        ; create debug window
        ;Gui, DebugMenu:New
        ;Gui, +AlwaysOnTop
        ;Gui, +ToolWindow

        ;static FakeJoinDetected
		;static MainLoginDetected
        ;static ServerListDetected
        ;static GameSelection
        ;static ModScreen
        ;static ConnectionFailed
        ;static JoiningFailed
        ;static NetworkFailed
        ;static JoinActive

        ;Gui, DebugMenu:Add, CheckBox, vFakeJoinDetected, fake join
		;Gui, DebugMenu:Add, CheckBox, vMainLoginDetected, main login
        ;Gui, DebugMenu:Add, CheckBox, vServerListDetected, serverlist
        ;Gui, DebugMenu:Add, CheckBox, vGameSelection, game selection
        ;Gui, DebugMenu:Add, CheckBox, vModScreen, mod screen
        ;Gui, DebugMenu:Add, CheckBox, vConnectionFailed, connection failed
        ;Gui, DebugMenu:Add, CheckBox, vJoiningFailed, joining failed
        ;Gui, DebugMenu:Add, CheckBox, vNetworkFailed, network failed
        ;Gui, DebugMenu:Add, Text, , ------------------
        ;Gui, DebugMenu:Add, CheckBox, vJoinActive, join btn active
        ;Gui, DebugMenu:Show, w150 h190, JoinSim : Debug

        ; game ui state 
        isFakeJoinDetected := 0
		isMainLoginDetected := 0
        isGameSelection := 0
        isJoinLastActive := 0
        isAtServerList := 0
        isAtModScreen := 0
        isAtConnectionFailed := 0
        isAtJoinFailed := 0
        isAtNetworkFailed := 0

        ; intent state
        isBackingOut := 0
        isWaitingJoinResult := 0

        while (true) {
            ; check for main login screen
            this.ArkWindow.CheckPixelColorGeneric(this.pix_homeScreen, MainLoginDetected, isMainLoginDetected)

            ; check for game selection screen
            this.ArkWindow.CheckPixelColorGeneric(this.pix_gameSelect, GameSelection, isGameSelection)

            ; check for server list
            this.ArkWindow.CheckPixelColorGeneric(this.pix_serverList, ServerListDetected, isServerListDetected)

            ; check for join button active
            this.ArkWindow.CheckPixelColorGeneric(this.pix_joinActive, JoinActive, isJoinActive)

            ; check for mod screen
            this.ArkWindow.CheckPixelColorGeneric(this.pix_modScreen, ModScreen, isAtModScreen)

            ; check for connection failed
            this.ArkWindow.CheckPixelColorGeneric(this.pix_conFailed, ConnectionFailed, isAtConnectionFailed)

            ; check for join failed
            this.ArkWindow.CheckPixelColorGeneric(this.pix_joinFailed, JoinFailed, isAtJoinFailed)

            ; check for network failure
            this.ArkWindow.CheckPixelColorGeneric(this.pix_netFailed, NetworkFailed, isAtNetworkFailed)
            
			; check for fake join message
			this.ArkWindow.CheckPixelColorGeneric(this.pix_FakeJoin, FakeJoin, isAtFakeJoin)
			;PixelGetColor, color, 1596, 696
			;MsgBox The color at that position is %color%.
			
            if (isMainLoginDetected) {
                this.ArkWindow.ClickArk(this.presstostartX, this.presstostartY) ; Press _ to start
            }

            if (isGameSelection) {
                this.ArkWindow.ClickArk(this.joingameX, this.joingameY) ; Join game
            }

            if (isServerListDetected && !isJoinActive) {
                if (!isBackingOut) {
                    this.ArkWindow.ClickArk(this.serverListTopServerX, this.serverListTopServerY) ; select the server
                } else {
                    this.ArkWindow.ClickArk(this.serverlistbackbuttonX, this.serverlistbackbuttonY) ; back

                    isBackingOut := 0
                }
            }

            if (isServerListDetected && isJoinActive) {
                this.ArkWindow.ClickArk(this.joinbuttonjX, this.joinbuttonjY) ; Join
            }

            if (isAtModScreen) {
                this.ArkWindow.ClickArk(this.modjoinbuttonjX, this.modjoinbuttonjY) ; Join

                isWaitingJoinResult := 1
            }

            if (isAtConnectionFailed) {
                this.ArkWindow.ClickArk(this.conFailedX, this.conFailedY) ; Cancel

                isBackingOut := 1
                isWaitingJoinResult := 0
            }

            if (isAtJoinFailed) {
                this.ArkWindow.ClickArk(this.joinFailedX, this.joinFailedY) ; OK

                isBackingOut := 1
                isWaitingJoinResult := 0
            }

            if (isAtNetworkFailed) {
                this.ArkWindow.ClickArk(this.netFailedX, this.netFailedY) ; OK

                isBackingOut := 1
                isWaitingJoinResult := 0
            }

			if (isAtFakeJoin) {
                this.ArkWindow.ClickArk(this.FakeJoinX, this.FakeJoinY) ; OK

                isBackingOut := 1
                isWaitingJoinResult := 0
            }
            Sleep 1
        }
    }

    InitCoords() {
        if (this.Is1080) {
			this.fakejoinX := 1019
			this.fakejoinY := 722
            this.presstostartX := 935
            this.presstostartY := 855
            this.joingameX := 682
            this.joingameY := 662
            this.serversearchboxX := 1615
            this.serversearchboxY := 195
            this.ServerUpBattleEyeLogoX := 110
            this.ServerUpBattleEyeLogoY := 330
            this.MultiplayerServers1X := 303
            this.MultiplayerServers1Y := 100
            this.joinbuttonjX := 1690
            this.joinbuttonjY := 945
            this.modjoinbuttonjX := 543
            this.modjoinbuttonjY := 921
            this.serverlistbackbuttonX := 172
            this.serverlistbackbuttonY := 875
            this.serverlistrefreshX := 965
            this.serverlistrefreshY := 940

            this.conFailedX := 978
            this.conFailedY := 730
            this.netFailedX := 869
            this.netFailedY := 369
            this.joinFailedX := 915
            this.joinFailedY := 391

            this.serverListTopServerX := 968
            this.serverListTopServerY := 331



            this.pix_homeScreen := { color: "0xFFFFFF", x: 1117, y: 858 }
            this.pix_gameSelect := { color: "0xFFEA86", x: 942, y: 960 }
            this.pix_serverList := { color: "0xFFFFFF", x: 774, y: 194 }
            this.pix_modScreen := { color: "0xCC800E", x: 1003, y: 201 }
            this.pix_joinActive := { color: "0xFFFFFF", x: 1734, y: 945 }
            this.pix_conFailed := { color: "0xFFF5C1", x: 852, y: 361 }
            this.pix_netFailed := { color: "0xFFF5C1", x: 852, y: 361 }
            this.pix_joinFailed := { color: "0x0000FF", x: 1160, y: 518 }
			this.pix_FakeJoin := { color: "0xC59B06", x: 1197, y: 522}
        } else if (this.Is1440) {
            this.fakejoinX := 1358
			this.fakejoinY := 963
			this.presstostartX := 1269
            this.presstostartY := 1148
            this.joingameX := 940
            this.joingameY := 815
            this.serversearchboxX := 2135
            this.serversearchboxY := 265
            this.ServerUpBattleEyeLogoX := 144
            this.ServerUpBattleEyeLogoY := 437
            this.MultiplayerServers1X := 424
            this.MultiplayerServers1Y := 139
            this.joinbuttonjX := 2255
            this.joinbuttonjY := 1268
            this.modjoinbuttonjX := 724
            this.modjoinbuttonjY := 1229
            this.serverlistbackbuttonX := 218
            this.serverlistbackbuttonY := 1180
            this.serverlistrefreshX := 1290
            this.serverlistrefreshY := 1250
            this.conFailedX := 1427
            this.conFailedY := 979
            this.netFailedX := 1124
            this.netFailedY := 487
            this.joinFailedX := 1213
            this.joinFailedY := 512
            this.serverListTopServerX := 1352
            this.serverListTopServerY := 437

            this.pix_homeScreen := { color: "0xFFFFFF", x: 1124, y: 1145 }
            this.pix_gameSelect := { color: "0xFFEA86", x: 1270, y: 1290 }
            this.pix_serverList := { color: "0xFFFFFF", x: 941, y: 258 }
            this.pix_modScreen := { color: "0xCC800E", x: 1338, y: 268 }
            this.pix_conFailed := { color: "0xFFF5C1", x: 1458, y: 483 }
            this.pix_netFailed := { color: "0xFFF5C1", x: 1458, y: 483 }
            this.pix_joinFailed := { color: "0x0000FF", x: 1160, y: 518 }
            this.pix_joinActive := { color: "0xFFFFFF", x: 2290, y: 1261 }
			this.pix_FakeJoin := { color: "0xC59B06", x: 1596, y: 696}
        }
    }
}

; This function absoutely sucks why did I code this GFN doesn't count mouse movements to their anti afk they need mouse down input.
AntiAfkMonitor:
    GuiControlGet, MonitorNumber, , MonitorNumber
    SysGet, Monitor, Monitor, % MonitorNumber
    MonitorX := MonitorLeft
    MonitorY := MonitorTop
    MonitorWidth := MonitorRight - MonitorLeft
    MonitorHeight := MonitorBottom - MonitorTop
    WinMove, on GeForce NOW,, %MonitorX%, %MonitorY%, %MonitorWidth%, %MonitorHeight%
    Sleep, 333
    Sleep, 240000
    ControlClick, x100 y100, ahk_exe ShooterGame.exe, , , , Pos
Return

MagicT:
    if (MagicTActive) {
        GuiControlGet, MagicDelay, , MagicDelay
        GuiControlGet, MagicKeybind, , MagicKeybind
        GuiControlGet, MagicTarget, , MagicTarget
        TargetExe := (MagicTarget = 1) ? "GeForceNOW.exe" : "ArkAscended.exe"
        KeyToSend := MagicKeybind = "T" ? "{T}" : MagicKeybind = "O" ? "{O}" : MagicKeybind = "F" ? "{F}" : MagicKeybind = "A" ? "{A}" : MagicKeybind = "E" ? "{E}" : ""
        
        if (MagicDelay < 1)
            MagicDelay := 1

        ; spam the key loop
        Loop {
            if not MagicTActive
                break
            
            ControlSend,, %KeyToSend%, ahk_exe %TargetExe%
            Sleep, % MagicDelay
        }
    }
    return


~*F3::
    bot := new SimBot()
    SimBot.Init()
    SimBot.Start()
return

~*F7::
reload
