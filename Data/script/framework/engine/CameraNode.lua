--[[
    desc: GameObject 挂一个Trans组件和一个Camera组件
    author:{hjy}
    time:2022-09-29 17:05:03
]]

local TransNode = require("framework.engine.TransNode")
local CameraComponent = require("framework.engine.CameraComponent")
local Math = require("Math")
local Engine = require("Engine")

local CameraNode = class("CameraNode", TransNode)

function CameraNode:ctor(scene, name)
  CameraNode.super.ctor(self, scene, name)
  self.camera_ = Engine.ToComCamera(self:AddComponent(CameraComponent):GetInternal())
end

function CameraNode:AttachRenderTarget(fbo)
  self.camera_:AttachRenderTarget(fbo);
end

function CameraNode:DetachRenderTarget(fbo)
  self.camera_:DetachRenderTarget(fbo)
end

function CameraNode:GetAttachedRenderTarget()
  return self.camera_:GetAttachedRenderTarget()
end

function CameraNode:DeletePostEffect(pe)
  self.camera_:DeletePostEffect(pe:GetPostEffect());
  pe.post = nil;
end

function CameraNode:Activate()
  self.camera_:Activate();
end

function CameraNode:Deactivate()
  self.camera_:Deactivate();
end

function CameraNode:SetSequence(seq)
  self.camera_:SetSequence(seq);
end

function CameraNode:GetSequence()
  return self.camera_:GetSequence();
end

function CameraNode:SetSequenceCulling(cull)
  self.camera_:SetSequenceCulling(cull);
end

function CameraNode:SetLayerMaskNothing()
  return self.camera_:SetLayerMaskNothing();
end

function CameraNode:AddLayerMask(mask)
  self.camera_:AddLayerMask(mask);
end

function CameraNode:EraseLayerMask(mask)
  self.camera_:EraseLayerMask(mask);
end

function CameraNode:CreateRealCameraProjection(n, f)
  self.camera_:CreateRealCameraProjection(n, f);
end

function CameraNode:CreatePerspectiveProjection(r, a, n, f)
  if nil == n or nil == f then
    self.camera_:CreatePerspectiveProjection(r, a);
  else    
    self.camera_:CreatePerspectiveProjection(r, a, n, f);
  end  
end

function CameraNode:CreateOrthographiProjection(w, h, n, f)
    if nil == n or nil == f then
    self.camera_:CreateOrthographiProjection(w, h);
  else    
    self.camera_:CreateOrthographiProjection(w, h, n, f);
  end  
end

function CameraNode:LookAt(pos, at, up)
  self.camera_:LookAt(pos, at, up);
end

function CameraNode:SetPosition(pos)
  self.camera_:SetPosition(pos);
end

function CameraNode:SetForward(f)
  self.camera_:SetForward(f);
end

function CameraNode:GetFov()
  return self.camera_:GetFov();
end

function CameraNode:SetFov(val)
  return self.camera_:SetFov(val);
end

function CameraNode:GetNear()
  return self.camera_:GetNear();
end

function CameraNode:SetNear(val)
  return self.camera_:SetNear(val);
end

function CameraNode:GetFar()
  return self.camera_:GetFar();
end

function CameraNode:SetFar(val)
  return self.camera_:SetFar(val);
end

function CameraNode:GetProjType()
  return self.camera_:GetProjType();
end

function CameraNode:SetProjType(val)
  return self.camera_:SetProjType(val);
end

function CameraNode:CreateProjection()
  return self.camera_:CreateProjection();
end

function CameraNode:PickRay(point)
  return self.camera_:PickRay(point);
end

function CameraNode:GetView()
  return self.camera_:GetView();
end

function CameraNode:GetProject()
  return self.camera_:GetProject();
end

function CameraNode:GetViewProj()
  return self.camera_:GetViewProj();
end

function CameraNode:isVisible(aabb)
  return self.camera_:isVisible(aabb);
end

function CameraNode:GetForward()
  return self.camera_:GetForward();
end

function CameraNode:GetUp()
  return self.camera_:GetUp();
end

function CameraNode:GetPosition()
  return self.camera_:GetPosition();
end

function CameraNode:Recalculate()
  return self.camera_:Recalculate();
end

function CameraNode:GetNode()
  return self.camera_;
end

function CameraNode:GetCameraResolution()
  return self.camera_:GetCameraResolution();
end

function CameraNode:SetClearFlag(flag)
  return self.camera_:SetClearFlag(flag);
end

function CameraNode:SetClearColor(color)
  self.camera_:SetClearColor(color);
end

function CameraNode:ChangeResolution(resolution)
  self.camera_:ChangeResolution(resolution);
end

return CameraNode