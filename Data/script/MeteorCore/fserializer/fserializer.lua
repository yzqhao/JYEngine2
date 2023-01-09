

local flatbuffers = require ("flatbuffers")
local TClass = require("vfbs.TClass")
local FEncoder = require "MeteorCore.fserializer.fencoder"
local FDecoder = require "MeteorCore.fserializer.fdecoder"
local Object = require "MeteorCore.core.object"


--Flatterbuffer 序列化
local FSerializer = class("FSerializer", Object)

function FSerializer:ctor()
  FSerializer.super.ctor();
  self.fbBuilder = flatbuffers.Builder(10000);
  
  self.flatEncoderArray = {};
  self.flatDecoderArray = {};
  self.encoderIndex = 0;
  self.decoderIndex = 0;
  self.encodeFinished = false;
  self.version = nil;
end


function FSerializer:GetEncoder()
  local encoder = nil;
  if #self.flatEncoderArray > self.encoderIndex then
    encoder = self.flatEncoderArray[self.encoderIndex];
  else
    encoder = FEncoder.new(self, self.fbBuilder);
    table.insert(self.flatEncoderArray,encoder);
  end
  self.encoderIndex = self.encoderIndex + 1;
  return encoder;
end

function FSerializer:GetDecoder(cls)
  local decoder = nil;
  if #self.flatDecoderArray > self.decoderIndex then
    decoder = self.flatDecoderArray[self.decoderIndex];
  else
    decoder = FDecoder.new(self);
    table.insert(self.flatDecoderArray,decoder);
  end
  decoder:Initialize(cls);
  self.decoderIndex = self.decoderIndex + 1;
  return decoder;
  
end

function FSerializer:GetDecoderFromBuffer(bufferAsString)
  local buf = flatbuffers.binaryArray.New(bufferAsString)
  local cls = TClass.GetRootAsTclass(buf,0);
  local decoder = self:GetDecoder(cls);
  
  local rDecoder = nil;
  local memberListCnt = decoder:GetLength();
  if memberListCnt == 2 then
    self.version = decoder:AsInt(1);  --第一个必然是版本号
    rDecoder = decoder:AsClass(2);
  else
    LOGE("Flatbuffer deserializer: Invalid buffer");
  end
  
  return rDecoder;
end

function FSerializer:GetVersion()
  return self.version;
end



--得到Buffer
function FSerializer:GetBuffer()
  local bufAsString = nil;
  if self.encodeFinished == false then
    self.encodedFinished = true;
    if self.encoderIndex > 0 then
      local memberList = self.flatEncoderArray[1]:GetMemberList();
      local memberListCnt = #memberList;
      TClass.StartMembersVector(self.fbBuilder,memberListCnt);
      for i = memberListCnt, 1, -1 do
        self.fbBuilder:PrependUOffsetTRelative(memberList[i]);
      end
      local members = self.fbBuilder:EndVector(memberListCnt);
        
      TClass.Start(self.fbBuilder);
      TClass.AddMembers(self.fbBuilder,members);
      local class = TClass.End(self.fbBuilder);
      
      
      self.fbBuilder:Finish(class);
      bufAsString = self.fbBuilder:Output();
    end
  end
  return bufAsString;
end

return FSerializer;