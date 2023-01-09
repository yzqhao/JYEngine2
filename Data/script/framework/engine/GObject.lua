--[[
    desc:{gameobject扩展}
    author:{hjy}
    time:2022-10-31 14:59:46
]]


local Math = require("Math")
local Engine = require("Engine")
local Object = require("MeteorCore.core.Object")

function Engine.GObject:CreateCameraComponent()
  local com = self:CreateComponent("CameraComponent");
  return Engine.ToComCamera(com)
end

function Engine.GObject:CreateLightComponent()
  local com = self:CreateComponent("LightComponent");
  return Engine.ToComLight(com)
end

function Engine.GObject:CreateRenderComponent()
  local com = self:CreateComponent("RenderComponent");
  return Engine.ToComRender(com)
end

function Engine.GObject:CreateTransformComponent()
  local com = self:CreateComponent("TransformComponent");
  return Engine.ToComTransform(com)
end

function Engine.GObject:GetCameraComponent()
  local com = self:GetComponent("CameraComponent");
  return Engine.ToComCamera(com)
end

function Engine.GObject:GetLightComponent()
  local com = self:GetComponent("LightComponent");
  return Engine.ToComLight(com)
end

function Engine.GObject:GetRenderComponent()
  local com = self:GetComponent("RenderComponent");
  return Engine.ToComRender(com)
end

function Engine.GObject:GetTransformComponent()
  local com = self:GetComponent("TransformComponent");
  return Engine.ToComTransform(com)
end
