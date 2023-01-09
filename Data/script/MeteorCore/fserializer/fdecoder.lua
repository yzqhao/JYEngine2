

local Object = require "MeteorCore.core.object"

local Type = require ("vfbs.Type")
local Member = require ("vfbs.Member")
local TBool = require ("vfbs.TBool")
local TString = require ("vfbs.TString")
local TInt = require("vfbs.TInt")
local TFloat = require ("vfbs.TFloat")
local TDouble = require ("vfbs.TDouble")
local TClass = require("vfbs.TClass")
local TReference = require("vfbs.TRefrence")

local FDATA_TYPE = 
{
  DT_BOOL = 0,
  DT_INT = 1,
  DT_FLOAT = 2,
  DT_DOUBLE = 3,
  DT_STRING = 4,
  DT_CLASS = 5,
  DT_REFERENCE = 6
};

local FDecoder = class("FDecoder", Object)

function FDecoder:ctor(serializer)
  self.serializer = serializer;
  self.memberList = {};
end

function FDecoder:GetVersion()
  return self.serializer:GetVersion();
end

function FDecoder:Initialize(cls)
  local memberListLength = cls:MembersLength();
  for i = 1, memberListLength do
    local member = cls:Members(i);
    table.insert(self.memberList,member);
  end
end

--key 不一定是string,对于lua中的数组类型key的类型是int
function FDecoder:GetFieldName(index)
  local result = nil;
  local member = self.memberList[index];
  local keyType = member:KeyType();
  if keyType == Type.TInt then
    local unionInt = TInt:New();
    local key = member:Key()
    unionInt:Init(key.bytes, key.pos);
    result = unionInt:Value();
  else
    local unionString = TString.New();
    local key = member:Key()
    unionString:Init(key.bytes, key.pos);
    result = unionString:Value();
  end
  return result;
end

function FDecoder:GetFieldType(index)
  local member = self.memberList[index];
  local valueType = member:ValueType();
  local ftype = nil;
  if valueType == Type.TBool then
    ftype = FDATA_TYPE.DT_BOOL;
  elseif valueType == Type.Int then
    ftype = FDATA_TYPE.DT_INT;
  elseif valueType == Type.TFloat then
    ftype = FDATA_TYPE.DT_FLOAT;
  elseif valueType == Type.TString then
    ftype = FDATA_TYPE.DT_STRING;
  elseif valueType == Type.TDouble then
    ftype = FDATA_TYPE.DT_DOUBLE;
  elseif valueType == Type.TClass then
    ftype = FDATA_TYPE.DT_CLASS;
  elseif valueType == Type.TRefrence then
    ftype = FDATA_TYPE.DT_REFERENCE;
  else
    LOGE("Unknow type");
  end
  return ftype;
end

function FDecoder:AsBool(index)
  local member = self.memberList[index];
  local unionBool = TBool.New();
  local value = member:Value()
  unionBool:Init(value.bytes,value.pos);
  local result = unionBool:Value();
  return result;
end

function FDecoder:AsString(index)
  local member = self.memberList[index];
  local unionString = TString.New();
  local value = member:Value() ;
  unionString:Init(value.bytes,value.pos);
  local result = unionString:Value();
  return result;
end

function FDecoder:AsReference(index)
  local member = self.memberList[index];
  local unionString = TReference.New();
  local value = member:Value()
  unionString:Init(value.bytes,value.pos);
  local result = unionString:Value();
  return result;
end

function FDecoder:AsInt(index)
  local member = self.memberList[index];
  local unionInt = TInt:New();
  local value = member:Value()
  unionInt:Init(value.bytes, value.pos);
  local result = unionInt:Value();
  return result;
end

function FDecoder:AsFloat(index)
  local member = self.memberList[index];
  local unionFloat = TFloat:New();
  local value = member:Value()
  unionFloat:Init(value.bytes,value.pos);
  local result = unionFloat:Value();
  return result;
end

function FDecoder:AsDouble(index)
  local member = self.memberList[index];
  local unionDouble = TDouble:New();
  local value = member:Value()
  unionDouble:Init(value.bytes,value.pos);
  local result = unionDouble:Value();
  return result;
end

function FDecoder:AsClass(index)
  local member = self.memberList[index];
  local cls = TClass.New();
  local value = member:Value()
  cls:Init(value.bytes,value.pos);
  local result = self.serializer:GetDecoder(cls);
  return result;
end

function FDecoder:GetLength()
  local count = 0;
  for key,value in pairs(self.memberList) do
    count = count + 1;
  end
  return count;
  --return #self.memberList;
end



return FDecoder; 