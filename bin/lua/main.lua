------------------------------------------------------------------------------------------------------------------------------------------
local os 		= os
local print 	= print
local require 	= require
local Network	= Network
------------------------------------------------------------------------------------------------------------------------------------------
-- require "test"
------------------------------------------------------------------------------------------------------------------------------------------
local protobuf = require "protobuf"
------------------------------------------------------------------------------------------------------------------------------------------
local addr = io.open("./test.pb", "rb");
local msg_buffer = addr:read("*a")
addr:close()
protobuf.register(msg_buffer)
--[[
t = protobuf.decode("google.protobuf.FileDescriptorSet", msg_buffer)
proto = t.file[1]

print("-----------------------------------------")
print(proto.name)
print(proto.package)
print("-----------------------------------------")
message = proto.message_type
for _,v in ipairs(message) do
	print(v.name)
	for _,v in ipairs(v.field) do
		print("\t".. v.name .. " ["..v.number.."] " .. v.label)
	end
end
--]]
function msg_encode_bak(encode_msg, len, conn)
	print("msg_encode_bak", encode_msg, len)
	Network.sendmsg(encode_msg, len, conn)
end

function printTable(name, t, tab)
	if type(t) ~= "table" then
		print(name .. " = " .. tostring(t))
		return
	end

	print(tab .. name .. " = " .. tostring(t) .. " {")
	for k, v in pairs(t) do
		if type(v) == "table" then
			printTable(k, v, tab .. "\t")
		else
			print(tab .. "\t" .. k .. " = " .. tostring(v))
		end
	end
	print(tab .. "}")
end

function main_enter(msg, len, conn)
--[[
    print(msg)
	print("------------------------------")
	local decode_msg = protobuf.decode("tutorial.MsgInfo", msg, len);
	-- local decode_msg = protobuf.decode("tutorial.MsgInfo", msg); -- endoce string
	for k, v in pairs(decode_msg) do
		print(k, v)
	end
	print("Recv: " .. decode_msg.from .. " " .. decode_msg.name .. " " .. decode_msg.time)


	local new_msg = {
		time = os.clock(),
		name = "new send time",
		from = "Server",
	}
	
	protobuf.encode("tutorial.MsgInfo", new_msg, msg_encode_bak, conn)
--	local send_msg = protobuf.encode("tutorial.MsgInfo", new_msg); 	-- decode string
	Network.sendmsg(send_msg, 0, conn)
    --]]
	return 1
end

function main_error(msg)
	print("lua print: " .. msg)
	return 1
end

function yunfengtest()
	addr = io.open("./addressbook.pb","rb")
	buffer = addr:read "*a"
	addr:close()

	protobuf.register(buffer)

	--t = protobuf.decode("google.protobuf.FileDescriptorSet", buffer)

	--proto = t.file[1]

	--print(proto.name)
	--print(proto.package)

	--message = proto.message_type

	--for _,v in ipairs(message) do
	--	print(v.name)
	--	for _,v in ipairs(v.field) do
	--		print("\t".. v.name .. " ["..v.number.."] " .. v.label)
	--	end
	--end

	addressbook = {
		name = "Alice",
		id = 12345,
		phone = {
			{ number = "1301234567" },
			{ number = "87654321", type = "WORK" },
		},
		test = {1},
		onePhone = {number = "87654321", type = "WORK"},
		intTest	= {1, 2,3,3, 4,4, 5,},
		ChildTest = {
			{
				number = 'num 1',
				phone = {{ number = "87654321", type = "WORK" }},
			},
			{
				number = 'num 2',
				phone = {{ number = "asfas", type = "WORK" }, { number = "asfas12", type = "WORK" }},
			},
			{
				number = 'num 3',
				phone = {{ number = "saaass"}},
			},
		}
	}

	code = protobuf.encode("tutorial.Person", addressbook)
	decode = protobuf.decode("tutorial.Person" , code)
    printTable("decode", decode, "")

	print(decode.name)
	print(decode.id)

end

yunfengtest()




--printTable("createTable", InterfaceTest.createTable({x = 1, y = 2}), "")


