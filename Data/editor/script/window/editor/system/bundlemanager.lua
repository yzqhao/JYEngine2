--[[
    desc:{管理序列化与反序列化}
    author:{hjy}
    time:2022-10-27 17:03:59
]]

local venusjson = require "venusjson"
local Math = require "Math"
local Engine = require "Engine"
local Core = require "MeteorCore"
local Object = require("MeteorCore.core.Object")

local BundleManager = {};

function BundleManager:SaveScene(path)
  local scene = Engine.SceneManager:GetEditScene(); --从scene开始序列化
  local stream = Core.Stream();
  stream:SetStreamFlag(Core.ArchiveType.AT_REGISTER);
  stream:ArchiveAll(scene);
  stream:Save(path);
  --[[local buffer = BundleSystem:Serialize(scene);
  local file = io.open(path,"wb");
  if file then
    file:write(buffer);
    file:close();
    local metapath = path .. ".meta";
    local result = {};
    result["filetype"] = "bundle";
    result["bundlepath"] = path;
    result["dependencies"] = BundleManager:GetDependenciesInNode(scene);
    result["Version"] = defined.SerializeVersion;
    result["AdvancedShading"] = apollocore.Framework:IsEditorAdvancedShading();
    filelfs:SaveFileInfo(result,metapath);
    filelfs:AppendToFileTree(path);
  end]]
end

function BundleManager:Deserialize(file, EClass)
  local stream = Core.Stream();
	stream:Load(file);
  return stream:GetObjectByRtti(EClass:RTTI());
end

return BundleManager;