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

function msg_encode_bak(encode_msg, len, conn)
	print("msg_encode_bak", encode_msg, len)
	Network.sendmsg(encode_msg, len, conn)
end


function main_enter(msg, len, conn)
	print(msg)
	print("------------------------------")
	local decode_msg = protobuf.decode("tutorial.MsgInfo", msg, len);
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

	t = protobuf.decode("google.protobuf.FileDescriptorSet", buffer)

	proto = t.file[1]

	print(proto.name)
	print(proto.package)

	message = proto.message_type

	for _,v in ipairs(message) do
		print(v.name)
		for _,v in ipairs(v.field) do
			print("\t".. v.name .. " ["..v.number.."] " .. v.label)
		end
	end

	addressbook = {
		name = "Alice",
		id = 12345,
		phone = {
			{ number = "1301234567" },
			{ number = "87654321", type = "WORK" },
		}
	}

	code = protobuf.encode("tutorial.Person", addressbook)
	decode = protobuf.decode("tutorial.Person" , code)

	print(decode.name)
	print(decode.id)

end

print("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")
yunfengtest()

