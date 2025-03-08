MaxThreadsPerHotkey 3

Toggle := true  ; Initialize Toggle to true to start the loop
Var := 0
Var2 := 0
Loop
{
    If (!Toggle)
    {
        Break
    }
    if (Var < 1)
    {
        Click "676133"
        MouseMove 509, 274
        sleep 400
        Send {a down}
        sleep 2000
        Send {a up}
        Send {a down}
        sleep 2000
        Send {a up}
        Send {a down}
        sleep 2000
        Send {a up}
        sleep 100000
        Click "448132"
        sleep 400
        Click "404203"
        sleep 400
        Click "833731"
        sleep 400
        Var++
        Var2++
    }
    else
    {
        Send {Esc}
        sleep 300
        Send {1}
        sleep 300
        if (Var2 > 15)
        {
            Send {2}
            sleep 300
            Send {3}
            sleep 300
            Var2 := 0
        }
        Send {i}
        sleep 300
        Var := 0
    }
}
Return
