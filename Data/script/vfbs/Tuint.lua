--[[ vfbs.Tuint

  Automatically generated by the FlatBuffers compiler, do not modify.
  Or modify. I'm a message, not a cop.

  flatc version: 22.9.29

  Declared by  : //generic.fbs
  Rooting type : vfbs.TClass (//generic.fbs)

--]]

local flatbuffers = require('flatbuffers')

local Tuint = {}
local mt = {}

function Tuint.New()
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
    return self.view:Get(flatbuffers.N.Uint32, self.view.pos + o)
  end
  return 0
end

function Tuint.Start(builder)
  builder:StartObject(1)
end

function Tuint.AddValue(builder, value)
  builder:PrependUint32Slot(0, value, 0)
end

function Tuint.End(builder)
  return builder:EndObject()
end

return Tuint