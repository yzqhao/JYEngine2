

local Object = require "MeteorCore.core.object"

local Type = require ("vfbs.Type")
local Member = require ("vfbs.Member")
local TString = require ("vfbs.TString")
local TInt = require ("vfbs.TInt")
local TFloat = require ("vfbs.TFloat")
local TDouble = require ("vfbs.TDouble")
local TBool = require ("vfbs.TBool")
local TClass = require("vfbs.TClass")
local TReference = require("vfbs.TRefrence")

local FEncoder = class("FEncoder", Object)

function FEncoder:ctor(serializer,builder)
  self.serializer = serializer;
  self.builder = builder;
  self.memberList = {};
end

function FEncoder:GetChild()
  return self.serializer:GetEncoder();
end

function FEncoder:GetMemberList()
  return self.memberList;
end

function FEncoder:SetTypename(typename)
  self:SetFieldString("typename",typename);
end


function FEncoder:SetFieldReference(name,value)
  if value == "" then
    --空字符串不序列化与反序列化
    return;
  end
  
  local fkey = nil;
  local ftype = nil;
  local keytype = type(name);
  if keytype == "number" then
    local cvalue = name;
    TInt.Start(self.builder);
    TInt.AddValue(self.builder,cvalue);
    fkey = TInt.End(self.builder);
    ftype = Type.TInt;
  else --默认都当string处理
    local ckey = self.builder:CreateString(name);
    TString.Start(self.builder);
    TString.AddValue(self.builder,ckey);
    fkey = TString.End(self.builder);
    ftype = Type.TString;
  end

  --value
  local cvalue = self.builder:CreateString(value);
  TString.Start(self.builder);
  TString.AddValue(self.builder,cvalue);
  local fvalue = TString.End(self.builder);
  --member
  Member.Start(self.builder);
  Member.AddKeyType(self.builder,ftype);
  Member.AddKey(self.builder,fkey);
  Member.AddValueType(self.builder,Type.TRefrence);
  Member.AddValue(self.builder,fvalue);
  local member = Member.End(self.builder);
  table.insert(self.memberList,member);
end


function FEncoder:SetFieldString(name,value)
  if value == "" then
    --空字符串不序列化与反序列化
    return;
  end
  
  local fkey = nil;
  local ftype = nil;
  local keytype = type(name);
  if keytype == "number" then
    local cvalue = name;
    TInt.Start(self.builder);
    TInt.AddValue(self.builder,cvalue);
    fkey = TInt.End(self.builder);
    ftype = Type.TInt;
  else --默认都当string处理
    local ckey = self.builder:CreateString(name);
    TString.Start(self.builder);
    TString.AddValue(self.builder,ckey);
    fkey = TString.End(self.builder);
    ftype = Type.TString;
  end
  
  --key
  --[[
  local ckey = self.builder:CreateString(name);
  TString.Start(self.builder);
  TString.AddValue(self.builder,ckey);
  local fkey = TString.End(self.builder);
  ]]--
  --value
  local cvalue = self.builder:CreateString(value);
  TString.Start(self.builder);
  TString.AddValue(self.builder,cvalue);
  local fvalue = TString.End(self.builder);
  --member
  Member.Start(self.builder);
  Member.AddKeyType(self.builder,ftype);
  Member.AddKey(self.builder,fkey);
  Member.AddValueType(self.builder,Type.TString);
  Member.AddValue(self.builder,fvalue);
  local member = Member.End(self.builder);
  table.insert(self.memberList,member);
end


function FEncoder:SetFieldInt(name,value)
  local fkey = nil;
  local ftype = nil;
  local keytype = type(name);
  if keytype == "number" then
    local cvalue = name;
    TInt.Start(self.builder);
    TInt.AddValue(self.builder,cvalue);
    fkey = TInt.End(self.builder);
    ftype = Type.TInt;
  else --默认都当string处理
    local ckey = self.builder:CreateString(name);
    TString.Start(self.builder);
    TString.AddValue(self.builder,ckey);
    fkey = TString.End(self.builder);
    ftype = Type.TString;
  end
  
  --key
  --[[
  local ckey = self.builder:CreateString(name);
  TString.Start(self.builder);
  TString.AddValue(self.builder,ckey);
  local fkey = TString.End(self.builder);
  ]]--
  --value
  local cvalue = value;
  TInt.Start(self.builder);
  TInt.AddValue(self.builder,cvalue);
  local fvalue = TInt.End(self.builder);
  --member
  Member.Start(self.builder);
  Member.AddKeyType(self.builder,ftype);
  Member.AddKey(self.builder,fkey);
  Member.AddValueType(self.builder,Type.TInt);
  Member.AddValue(self.builder,fvalue);
  
  local member = Member.End(self.builder);
  table.insert(self.memberList,member);
end


function FEncoder:SetFieldFloat(name,value)
  local fkey = nil;
  local ftype = nil;
  local keytype = type(name);
  if keytype == "number" then
    local cvalue = name;
    TInt.Start(self.builder);
    TInt.AddValue(self.builder,cvalue);
    fkey = TInt.End(self.builder);
    ftype = Type.TInt;
  else --默认都当string处理
    local ckey = self.builder:CreateString(name);
    TString.Start(self.builder);
    TString.AddValue(self.builder,ckey);
    fkey = TString.End(self.builder);
    ftype = Type.TString;
  end
  
  --key
  --[[
  local ckey = self.builder:CreateString(name);
  TString.Start(self.builder);
  TString.AddValue(self.builder,ckey);
  local fkey = TString.End(self.builder);
  ]]--
  --value
  local cvalue = value;
  TFloat.Start(self.builder);
  TFloat.AddValue(self.builder,cvalue);
  local fvalue = TFloat.End(self.builder);
  --member
  Member.Start(self.builder);
  Member.AddKeyType(self.builder,ftype);
  Member.AddKey(self.builder,fkey);
  Member.AddValueType(self.builder,Type.TFloat);
  Member.AddValue(self.builder,fvalue);
  
  local member = Member.End(self.builder);
  table.insert(self.memberList,member);
end

function FEncoder:SetFieldDouble(name,value)
  
  local fkey = nil;
  local ftype = nil;
  local keytype = type(name);
  if keytype == "number" then
    local cvalue = name;
    TInt.Start(self.builder);
    TInt.AddValue(self.builder,cvalue);
    fkey = TInt.End(self.builder);
    ftype = Type.TInt;
  else --默认都当string处理
    local ckey = self.builder:CreateString(name);
    TString.Start(self.builder);
    TString.AddValue(self.builder,ckey);
    fkey = TString.End(self.builder);
    ftype = Type.TString;
  end
  
  --key
  --[[
  local ckey = self.builder:CreateString(name);
  TString.Start(self.builder);
  TString.AddValue(self.builder,ckey);
  local fkey = TString.End(self.builder);
  --]]
  --value
  local cvalue = value;
  TDouble.Start(self.builder);
  TDouble.AddValue(self.builder,cvalue);
  local fvalue = TDouble.End(self.builder);
  --member
  Member.Start(self.builder);
  Member.AddKeyType(self.builder,ftype);
  Member.AddKey(self.builder,fkey);
  Member.AddValueType(self.builder,Type.TDouble);
  Member.AddValue(self.builder,fvalue);
  
  local member = Member.End(self.builder);
  table.insert(self.memberList,member);
end

--序列化不了bool类型？
function FEncoder:SetFieldBool(name,value)
  local fkey = nil;
  local ftype = nil;
  local keytype = type(name);
  if keytype == "number" then
    local cvalue = name;
    TInt.Start(self.builder);
    TInt.AddValue(self.builder,cvalue);
    fkey = TInt.End(self.builder);
    ftype = Type.TInt;
  else --默认都当string处理
    local ckey = self.builder:CreateString(name);
    TString.Start(self.builder);
    TString.AddValue(self.builder,ckey);
    fkey = TString.End(self.builder);
    ftype = Type.TString;
  end
  
  --key
  --[[
  local ckey = self.builder:CreateString(name);
  TString.Start(self.builder);
  TString.AddValue(self.builder,ckey);
  local fkey = TString.End(self.builder);
  ]]--
  --value
  local cvalue = value;
  TBool.Start(self.builder);
  TBool.AddValue(self.builder,cvalue);
  local fvalue = TBool.End(self.builder);
  
  Member.Start(self.builder);
  Member.AddKeyType(self.builder,ftype);
  Member.AddKey(self.builder,fkey);
  Member.AddValueType(self.builder,Type.TBool);
  Member.AddValue(self.builder,fvalue);
  
  local member = Member.End(self.builder);
  table.insert(self.memberList,member);
end


function FEncoder:SetFieldClass(name, encoder)
  local memberList = encoder:GetMemberList();
  local memberListCnt = #memberList;
  TClass.StartMembersVector(self.builder,memberListCnt);
  for i = memberListCnt, 1, -1 do
    self.builder:PrependUOffsetTRelative(memberList[i]);
  end
  local members = self.builder:EndVector(memberListCnt);
  --key
  --考虑到lua中的table，key值可能不是string
  local fkey = nil;
  local ftype = nil;
  local keytype = type(name);
  if keytype == "number" then
    local cvalue = name;
    TInt.Start(self.builder);
    TInt.AddValue(self.builder,cvalue);
    fkey = TInt.End(self.builder);
    ftype = Type.TInt;
  else --默认都当string处理
    local ckey = self.builder:CreateString(name);
    TString.Start(self.builder);
    TString.AddValue(self.builder,ckey);
    fkey = TString.End(self.builder);
    ftype = Type.TString;
  end

  
  --value
  TClass.Start(self.builder);
  TClass.AddMembers(self.builder,members);
  local subClass = TClass.End(self.builder);
  
  Member.Start(self.builder);
  Member.AddKeyType(self.builder,ftype);
  Member.AddKey(self.builder,fkey);
  Member.AddValueType(self.builder,Type.TClass);
  Member.AddValue(self.builder,subClass);
  local member = Member.End(self.builder);
  table.insert(self.memberList,member);
end


return  FEncoder;