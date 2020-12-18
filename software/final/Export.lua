------------------------
-- Export plugin for SimTools V3
-- Version 1.0
-- Export start
------------------------

Myfunction =

{
Start=function(self)
    package.path = package.path..";.\\LuaSocket\\?.lua"
    package.cpath = package.cpath..";.\\LuaSocket\\?.dll"
    socket = require("socket")
    default_output_file = io.open(lfs.writedir().."/Logs/test.txt", "w")
    io.output(file)
    print("startworking")
    my_init = socket.protect(function()
        -- export telemetry to SimTools
        host1 = host1 or "169.254.170.171"
        port1 = port1 or 8888
        c = socket.udp ( )
        c:settimeout ( 1 )
        c:setsockname('*',0)
        c:setpeername ( "127.0.0.1", 8888 )
        assert(c:send ( "Hello Wolrd!" ))
      --  c1 = socket.try(socket.connect(host1, port1)) -- connect to the listener socket
      --  c1:setoption("tcp-nodelay",true) -- set immediate transmission mode
      --  c1:settimeout(.01)
    end)
    my_init()
end,


AfterNextFrame=function(self)

 local time = LoGetModelTime()

    local selfData = LoGetSelfData()

    local pitch = selfData.Pitch
    --local roll = selfData.Bank

    -- Angular velocity euler angles (in rad/s)
    -- x (around roll axis, [leftRoll:-PI/s, rightRoll:+PI/s[)
    -- y (around yaw axis, [leftYaw:+PI/s, rightYaw:-PI/s[)

    -- z (around pitch axis, [downPitch:-PI/s, upPitch:+PI/s[)

    local angularVelocity = LoGetAngularVelocity()

    -- Acceleration (G)
    local acceleration = LoGetAccelerationUnits()

    local yaw = 0 --selfData.Heading
    --local pitch = acceleration.x

    local roll = acceleration.z

    -- Air Data
    local ias = LoGetIndicatedAirSpeed()
    local aoa = LoGetAngleOfAttack();

    -- Weight On Wheels
    local leftGear = LoGetAircraftDrawArgumentValue(6)
    local noseGear = LoGetAircraftDrawArgumentValue(1)
    local rightGear = LoGetAircraftDrawArgumentValue(4)

    --local sensor_data = get_base_data()
    local rpm = 0 -- LoGetEngineInfo.RPM.left

    -- onGround status is determined by the compression of ANY gear strut
    local flags = 0
    if (leftGear > 0 or noseGear > 0 or rightGear > 0) then
        flags = flags + 1
    end




    --default_output_file:write(string.format("%.4f;%.4f;%.4f;%.4f;%.4f;%.4f;%.4f;\n", time, angularVelocity.x, angularVelocity.y, angularVelocity.z, acceleration.x, acceleration.y, acceleration.z))
    local socket_1 = require("socket")
    local udp = assert(socket_1.udp())

    acceleration.x=acceleration.x*35*(3.14159/180) --mapping
    local output = (pitch + acceleration.x) * 0.5
    if acceleration.x > 1 or acceleration.x < -1 then
      output = 0
    end

    default_output_file:write(string.format("%.4f;%.4f;\n", time, output))


    udp:settimeout(1)
    assert(udp:setoption('broadcast', true))
    assert(udp:setoption('dontroute',true))
    assert(udp:setsockname("*",0))
    assert(udp:setpeername( "127.0.0.1", 8888 ))
    assert(udp:send(string.format("%.2f  %.3f",output,time)))
    --assert(udp:send(string.format("%.2f  %.3f  %.2f",output,time,acceleration.z)))
    --  data = udp:receive()


    my_send = socket.protect(function()
        if c then
            -- socket.try(c:send(string.format("%.4f;%.4f;%.4f;%.4f;%.4f;%.4f;%.4f;%.4f;%.4f;%.4f;%.4f;%d;%d;\n", time, yaw, pitch, roll, angularVelocity.x, angularVelocity.y, angularVelocity.z, acceleration.x, acceleration.y, acceleration.z, ias,rpm, flags)))
             --socket.try(c:send(string.format("%.4f;%.4f;\n", time, pitch)))
            end
    end)
    my_send()

end,


Stop=function(self)

    if default_output_file then
	     default_output_file:close()
	     default_output_file = nil
    end
    my_close = socket.protect(function()
        if c then
            c:close()
        end
    end)
    my_close()
end
}


-- =============
-- Overload
-- =============

-- Works once just before mission start.
do
    local PrevLuaExportStart=LuaExportStart
    LuaExportStart=function()
        Myfunction:Start()
        if PrevLuaExportStart then
            PrevLuaExportStart()
        end
    end
end

-- Works just after every simulation frame.
do
    local PrevLuaExportAfterNextFrame=LuaExportAfterNextFrame
    LuaExportAfterNextFrame=function()
        Myfunction:AfterNextFrame()
        if PrevLuaExportAfterNextFrame then
            PrevLuaExportAfterNextFrame()
        end
    end
end

-- Works once just after mission stop.
do
    local PrevLuaExportStop=LuaExportStop
    LuaExportStop=function()
        Myfunction:Stop()
        if PrevLuaExportStop then
            PrevLuaExportStop()
        end
    end
end
