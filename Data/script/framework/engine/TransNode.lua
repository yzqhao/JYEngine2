--[[
    desc: GameObject 挂一个Transform组件
    author:{hjy}
    time:2022-09-29 16:37:12
]]

local GameObject = require("framework.engine.GameObject")
local TransformComponent = require("framework.engine.TransformComponent")
local Math = require("Math")
local Engine = require("Engine")

local TransNode = class("TransNode", GameObject)

function TransNode:ctor(scene, name)
    TransNode.super.ctor(self, scene, name)
    self.position_ = Math.Vec3();
    self.scale_ = Math.Vec3();
    self.trans_ = Engine.ToComTransform(self:AddComponent(TransformComponent):GetInternal())
end

function TransNode:SetLocalPosition(pos_x, y, z)
  if y and z then
    self.position_:Set(pos_x, y, z);
  else
    self.position_:Set(pos_x.x, pos_x.y, pos_x.z);
  end
  self.trans_:SetLocalPosition(self.position_);
end

function TransNode:SetLocalScale(s_x,y,z)
  if y and z then
    self.scale_:Set(s_x, y, z);
  else
    self.scale_:Set(s_x.x, s_x.y, s_x.z);
  end     
  self.trans_:SetLocalScale(self.scale_);
end

function TransNode:SetLocalRotation(r)
  self.trans_:SetLocalRotation(r);
end

function TransNode:GetLocalPosition()
  return self.position_;
end

function TransNode:GetLocalScale()
  return self.trans_:GetLocalScale();
end

function TransNode:GetLocalRotation()
  return self.trans_:GetLocalRotation();
end

function TransNode:GetWorldTransform()
  return self.trans_:GetWorldTransform();
end

function TransNode:GetWorldScale()
  return self.trans_:GetWorldScale();
end

function TransNode:GetWorldPosition()
  return self.trans_:GetWorldPosition();
end

function TransNode:GetWorldRotation()
  return self.trans_:GetWorldRotation();
end

function TransNode:GetWorldScale()
  return self.trans_:GetWorldScale();
end

function TransNode:GetWorldRotation()
  return self.trans_:GetWorldRotation();
end

function TransNode:SetWorldPosition(wpos)
  self.trans_:SetWorldPosition(wpos);
  self.position_ = self.trans_:GetLocalPosition();
end

return TransNode