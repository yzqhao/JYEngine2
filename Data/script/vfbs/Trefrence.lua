--[[ vfbs.Trefrence

  Automatically generated by the FlatBuffers compiler, do not modify.
  Or modify. I'm a message, not a cop.

  flatc version: 22.9.29

  Declared by  : //generic.fbs
  Rooting type : vfbs.TClass (//generic.fbs)

--]]

local flatbuffers = require('flatbuffers')

local Trefrence = {}
local mt = {}

function Trefrence.New()
  local o = {}
  setmetatable(o, {__index = mt})
  return o
end

function mt:Init(buf, pos)
  self.view = flatbuffers.view.New(buf, pos)
end

function mt:Value()
  local o = self.view:Offset(4)
  if o ~= 0 then
    return self.view:String(self.view.pos + o)
  end
end

function Trefrence.Start(builder)
  builder:StartObject(1)
end

function Trefrence.AddValue(builder, value)
  builder:PrependUOffsetTRelativeSlot(0, value, 0)
end

function Trefrence.End(builder)
  return builder:EndObject()
end

return Trefrence