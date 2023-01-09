--[[
    desc: Object 
    author:{hjy}
    time:2022-09-28 19:11:54
]]
--require "functions"

local Object = class("Object")

function Object:ctor()
    
end


function Object:cast(obj)
  setmetatable(obj, self)
  obj.__index = Object
  return obj
end

return Object
