--This Export.lua illustrates how to export multiple data. In this case IAS (Indicated Airspeed) and AoA (Angle of Attack)

function LuaExportStart()

	package.path  = package.path..";"..lfs.currentdir().."/LuaSocket/?.lua"
	package.cpath = package.cpath..";"..lfs.currentdir().."/LuaSocket/?.dll"

	socket = require("socket")
	--IPAddress = "192.168.0.21"
	IPAddress = "127.0.0.1"
		--The IPAdress variable has to match the machine you're sending the data to. If you're sending the data to same machine as youre reading, use "127.0.0.1"
	Port = 31090

	MySocket = socket.try(socket.connect(IPAddress, Port))
	MySocket:setoption("tcp-nodelay",true) 
end

function LuaExportBeforeNextFrame()
end

function LuaExportAfterNextFrame()

	--Both data are read:
	local ACC = LoGetAccelerationUnits();
	local pitch, roll, yaw = LoGetADIPitchBankYaw()
	local rotationSpeed = LoGetAngularVelocity()

	epoxy = GetDevice(6)
	if epoxy then
		--check functions 
		local meta = getmetatable(epoxy)
		f14=false
		if meta then
			local ind = getmetatable(epoxy)["__index"]
			if ind then 
				if ind["get_version"]~=nil and ind["get_variable_names"]~=nil then
					f14=true
					f14_n2i = {}
					f14_i2n = {}
					f14_variables = {}
					names = epoxy:get_variable_names()
					for i,v in ipairs(names) do
						f14_n2i[v] = i
						f14_i2n[i] = v

					end
				end
			end
		end
	end


	additionalData =""
	local epoxy = GetDevice(6)
	if epoxy ~= nil and type(epoxy) ~= "number" and f14_i2n ~= nil then 
		local values = epoxy:get_values()
		for i,v in ipairs(values) do
			f14_variables[f14_i2n[i]] = v
			additionalData = additionalData .. f14_i2n[i] .. "=" .. v .. ";"
			--socket.try(MySocket:send(string.format("%.4f:%.4f:%.4f\n",ACC.x,ACC.y,ACC.z)))

		end
	end


	--And then both data are being written into the formatted string:

	--socket.try(MySocket:send(string.format("%.4f:%.4f:%.4f\n",ACC.x,ACC.y,ACC.z)))
	socket.try(MySocket:send(additionalData))

end

function LuaExportStop()

	if MySocket then 
		socket.try(MySocket:send("exit"))
		MySocket:close()
	end
end

function LuaExportActivityNextEvent(t)
end
