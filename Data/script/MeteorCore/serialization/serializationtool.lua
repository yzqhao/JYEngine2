local Serializer = require "MeteorCore.fserializer.fserializer"
local FDatatype = require "MeteorCore.fserializer.fdatatype"

local Core = require "MeteorCore"

local SerializationTool = {};

function SerializationTool:GetAttribute(decoder, parentName)
  local attributes = {};
  local typeName = decoder:AsString(1);
  local isTable = false;
  attributes["__typename"] = typeName;
  if typeName == "table" then
    isTable = true;
  end
  local memberListCnt = decoder:GetLength();
  for i = 2, memberListCnt do
    local fieldName = decoder:GetFieldName(i);
    local fieldType = decoder:GetFieldType(i);
    local fieldValue = nil;
    if fieldType == FDatatype.DT_BOOL then
      fieldValue = decoder:AsBool(i);
    elseif fieldType == FDatatype.DT_INT then
      fieldValue = decoder:AsInt(i);
    elseif fieldType == FDatatype.DT_FLOAT then
      fieldValue = decoder:AsFloat(i);
    elseif fieldType == FDatatype.DT_DOUBLE then
      fieldValue = decoder:AsDouble(i);
    elseif fieldType == FDatatype.DT_STRING then
      fieldValue = decoder:AsString(i);
    elseif fieldType == FDatatype.DT_CLASS then
      local childDecoder = decoder:AsClass(i);
      fieldValue = self:GetAttribute(childDecoder, fieldName);
    elseif fieldType == FDatatype.DT_REFERENCE then
      fieldValue = {};
      fieldValue["__typename"] = Core.ScriptTypes.ReferenceTypeName;
      fieldValue["refid"] = decoder:AsString(i);
      fieldValue["fieldName"] = fieldName;
      fieldValue["fieldCount"] = memberListCnt - 1;
      fieldValue["parentName"] = parentName;
      fieldValue["number"] = i - 1;
    end
    if fieldValue ~= nil then
      attributes[fieldName] = fieldValue;
    end
  end
  return attributes;
end

function SerializationTool:_SerializeCls(cls, encoder)
  --local fieldName = cls["fieldname"];
  local typename = cls["typename"];
  local members = cls["members"];
  local value = cls["value"];
  local __reference_uuid = cls["__reference_uuid"];
  --encoder:SetFieldString("__typename", typename);  --类型
  encoder:SetTypename(typename);
  if __reference_uuid then
    encoder:SetFieldString("__reference_uuid", __reference_uuid);
  end
  --local memberCnt = #members;
  --for i = 1, memberCnt do

  --这里合并有冲突  node克隆素材和prefab编辑的逻辑
  for childFieldName, member in pairs(members) do
    --local member = members[i];
    --local memberFieldName = member["fieldname"];
    local memberTypeName = member["typename"];
    local memberValue = member["value"];
    local childMembers = member["members"];
    if memberTypeName ~= "number"
            and memberTypeName ~= "boolean"
            and memberTypeName ~= "string"
            and memberTypeName ~= Core.ScriptTypes.ReferenceTypeName then
      --class 类型
      local childEncoder = encoder:GetChild();
      --childFieldName = self:_SerializeCls(member, childEncoder);  --class field 给一个key name
      self:_SerializeCls(member, childEncoder);  --class field 给一个key name
      encoder:SetFieldClass(childFieldName, childEncoder);
    else
      if memberTypeName == "number" then
        encoder:SetFieldDouble(childFieldName, memberValue);
      elseif memberTypeName == "boolean" then
        encoder:SetFieldBool(childFieldName, memberValue);
      elseif memberTypeName == "string" then
        encoder:SetFieldString(childFieldName, memberValue);
      elseif memberTypeName == Core.ScriptTypes.ReferenceTypeName then
        encoder:SetFieldReference(childFieldName, memberValue);
        --encoder:SetFieldReference(memberFieldName, memberValue,memberCnt-1,i - 1,fieldName);
      end
    end
  end
end

function SerializationTool:GetDecoder(bufferAsString)
  local serializer = Serializer();
  local decoder = serializer:GetDecoderFromBuffer(bufferAsString);
  return decoder, serializer:GetVersion();
end

return SerializationTool;