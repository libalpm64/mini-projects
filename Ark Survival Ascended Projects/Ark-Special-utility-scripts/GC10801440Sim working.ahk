#SingleInstance Force
CoordMode, Pixel, Screen
CoordMode, Mouse, Client

msgbox Start on any screen. You must manually join the server once - this uses the "Join Last" button. `nF1: Start F3: Close Script F4: Reload Script `nMake sure Nvidia filters are off while simming (Alt + Z click "Filters") and disable Digital Vibrance.i


global winTarget := "ArkAscended"

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
            this.modjoinbuttonjX := 328
            this.modjoinbuttonjY := 935
            this.serverlistbackbuttonX := 172
            this.serverlistbackbuttonY := 875
            this.serverlistrefreshX := 965
            this.serverlistrefreshY := 940

            this.conFailedX := 978
            this.conFailedY := 730
            this.joinFailedX := 915
            this.joinFailedY := 391

            this.serverListTopServerX := 968
            this.serverListTopServerY := 331



            this.pix_homeScreen := { color: "0xFFFFFF", x: 1117, y: 858 }
            this.pix_gameSelect := { color: "0xFFEA86", x: 942, y: 960 }
            this.pix_serverList := { color: "0xFFFFFF", x: 774, y: 194 }
            this.pix_modScreen := { color: "0xEBCDA0", x: 942, y: 209 }
            this.pix_joinActive := { color: "0xFFFFFF", x: 1734, y: 945 }
            this.pix_conFailed := { color: "0xFFF5C1", x: 852, y: 361 }
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
            this.modjoinbuttonjX := 436
            this.modjoinbuttonjY := 1245
            this.serverlistbackbuttonX := 218
            this.serverlistbackbuttonY := 1180
            this.serverlistrefreshX := 1290
            this.serverlistrefreshY := 1250
            this.conFailedX := 1427
            this.conFailedY := 979
            this.joinFailedX := 1213
            this.joinFailedY := 512
            this.serverListTopServerX := 1352
            this.serverListTopServerY := 437

            this.pix_homeScreen := { color: "0xFFFFFF", x: 1124, y: 1145 }
            this.pix_gameSelect := { color: "0xFFEA86", x: 1270, y: 1290 }
            this.pix_serverList := { color: "0xFFFFFF", x: 941, y: 258 }
            this.pix_modScreen := { color: "0xCC800E", x: 2211, y: 601 }
            this.pix_conFailed := { color: "0xFFF5C1", x: 1458, y: 483 }
            this.pix_joinFailed := { color: "0x0000FF", x: 1160, y: 518 }
            this.pix_joinActive := { color: "0xFFFFFF", x: 2290, y: 1261 }
			this.pix_FakeJoin := { color: "0xC59B06", x: 1596, y: 696}
        }
    }
}

F2::
    WinActivate, ArkAscended
    WinGetTitle, winTitle, A
    MouseGetPos, msX, msY, mWin
    PixelGetColor, pixCol, msX, msY

    msgbox color is %pixCol%, x: %msX%, y: %msY%, window: [%winTitle%]
return

F1::
    bot := new SimBot()
    SimBot.Init()
    SimBot.Start()
return

F3::
    ExitApp
	
F4::
reload
