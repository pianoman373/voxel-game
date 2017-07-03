local ffi = require("ffi")
ffi.cdef[[
void lua_registerBlock(int id, const char *name, int texX, int texY);
]]

function registerBlock(id, name, texX, texY)
	ffi.C.lua_registerBlock(id, name, texX, texY)
end

registerBlock(2, "Stone", 0, 0);
registerBlock(3, "Dirt", 4, 0);

registerBlock(4, "Cobblestone", 2, 0);
registerBlock(5, "Planks", 6, 0);
