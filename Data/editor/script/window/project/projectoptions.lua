local Core = require "MeteorCore"
local Serializer = require "MeteorCore.fserializer.fserializer"
local FDatatype = require "MeteorCore.fserializer.fdatatype"
local Object = require "MeteorCore.core.object"

--需要持久化的工程配置
local ProjectOptions = class("ProjectOptions", Object)

ProjectOptions.Version = 1; --版本号

function ProjectOptions:ctor()
  ProjectOptions.super.ctor();
  self.persistentConfig = {};
end

--工程持久性配置存储到文件中
function ProjectOptions:Serialize()
  local configPath = Core.IFileSystem:Instance():PathAssembly("root:config.dat");  --暂时存到引擎目录下
  local serializer = Serializer.new();
  local encoder = serializer:GetEncoder();
  local serializeInfo = self:_SerializeTraverse(self.persistentConfig);
  local options = serializeInfo[1];
  if options then
    encoder:SetFieldInt("version",ProjectOptions.Version);
    local childEncoder = encoder:GetChild();
    
    self:_SerializeCls(options,childEncoder);
    encoder:SetFieldClass("Data",childEncoder);
    
    local buffAsString = serializer:GetBuffer();  --返回序列化结果
    local file = io.open(configPath,"wb");
    file:write(buffAsString);
    file:close();
  end
  
  
end

--读取工程配置
function ProjectOptions:Deserialize()
  
  local configPath = Core.IFileSystem:Instance():PathAssembly("root:config.dat");
  local file = io.open(configPath, "rb");
  if file then
    local bufferAsString = file:read("*a");
  
    local serializer = Serializer.new();
    local decoder = serializer:GetDecoderFromBuffer(bufferAsString);
  
    self.persistentConfig = self:_DeSerializeOptions(decoder,nil);
  end
end

function ProjectOptions:_DeSerializeOptions(decoder)
  local memberListCnt = decoder:GetLength();

  local typename = decoder:AsString(1);
  local cls = {}; --这里的对象都是table
  local attributes = {};
  for i = 2, memberListCnt do
    local fieldName = decoder:GetFieldName(i);
    local fieldType = decoder:GetFieldType(i);
    local fieldValue = nil;
    --local isNewValue = true;  --标记是否是脚本中新创建的对象（有些在new父类的时候子类已经创建，就不需要再调用设置属性的函数了）
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
      fieldValue = self:_DeSerializeOptions(childDecoder);  --反序列化子类
    end
    if fieldValue then
      attributes[fieldName] = fieldValue;
    end
  end
  for name, attr in pairs(attributes) do
    cls[name] = attr;
  end
  return cls;
end



function ProjectOptions:_SerializeCls(cls,encoder)
  local fieldName = cls["fieldname"];
  local typename = cls["typename"];
  local members = cls["members"];
  local value = cls["value"];
  encoder:SetFieldString("typename",typename);  --类型
  local memberCnt = #members;
  for i = 1, memberCnt do
    local member = members[i];
    local childMembers = member["members"];
    local childMembersCnt = #childMembers;
    if childMembersCnt > 0 then  --class 类型
      local childEncoder = encoder:GetChild();
      local childFieldName = self:_SerializeCls(member,childEncoder);  --class field 给一个key name
      encoder:SetFieldClass(childFieldName,childEncoder);
    else
      local memberFieldName = member["fieldname"];
      local memberTypeName = member["typename"];
      local memberValue = member["value"];
      if memberTypeName == "number" then
        encoder:SetFieldDouble(memberFieldName,memberValue);
      elseif memberTypeName == "boolean" then
        encoder:SetFieldBool(memberFieldName,memberValue);
      elseif memberTypeName == "string" then
        encoder:SetFieldString(memberFieldName,memberValue);
      end
    end
  end
  return fieldName;
end

function ProjectOptions:_SerializeTraverse(table)
  local info = {};
  self:_SerializeTraverseOptions(table,nil,nil,info);
  return info;
end

function ProjectOptions:_SerializeTraverseOptions(obj,root,rootkey,info)
  local t = type(obj);
  if t == "table" then  --脚本序列化
    local str = "table";
    local cls = {};
    cls["typename"] = str; cls["fieldname"] = rootkey or "";
    cls["members"] = cls["members"] or {};
    table.insert(info,cls);

    for key, handle in pairs(obj) do
      self:_SerializeTraverseOptions(obj[key],obj,key,cls["members"]);
    end
  elseif t == "number" or t == "boolean" or t == "string" then
    local cls = {};
    cls["typename"] = t; cls["fieldname"] = rootkey; cls["members"] = {}; cls["value"] = obj;
    table.insert(info,cls);
  end
end



return ProjectOptions;



 