--[[
    desc: GameObject 挂一个Trans组件和一个Render组件
    author:{hjy}
    time:2022-09-29 17:41:07
]]

local TransNode = require("framework.engine.TransNode")
local RenderComponent = require("framework.engine.RenderComponent")
local Math = require("Math")
local Engine = require("Engine")

local RenderNode = class("RenderNode", TransNode)

function RenderNode:ctor(scene, name)
  RenderNode.super.ctor(self, scene, name)
  local com = self:AddComponent(RenderComponent):GetInternal()
  self.render_ = Engine.ToComRender(com)
  self.isshow_ = true;
  self.iscaster_ = true;
  self.materials_ = {}
end

function RenderNode:AddMaterial(materialpath)
  local material = Engine.MaterialEntity()
  material:PushMetadata(
    Engine.MaterialMetadata(materialpath));
  material:CreateResource();
  self.render_:AddMaterialEntity(material);
  self.materials_[#self.materials_+1] = material
  return material
end

function RenderNode:RemoveMaterial(matIndex)
  table.remove(self.materials_, matIndex)
  self.render_:RemoveMaterialEntity(matIndex-1)
end

function RenderNode:CreateMesh(meshpath)
  self.render_:PushMetadata(Engine.RenderObjectMeshFileMetadate(Engine.RHIDefine.MU_STATIC, meshpath))
  self.render_:CreateResource();
end

function RenderNode:CreateRenderObj(rendermode, vertexstream, indicesstream)
  local vmeta = Engine.ReferenceVertexMetadata(Engine.RHIDefine.MU_STATIC, vertexstream)
  local imeta = Engine.ReferenceIndicesMetadata(Engine.RHIDefine.MU_STATIC, indicesstream)
  self.render_:PushMetadata(Engine.RenderObjectMeshMetadate( 
    rendermode,
    vmeta,
    imeta))
  self.render_:CreateResource();
end

function RenderNode:SetParameter(slot, value)
  if value then
    self.render_:SetParameter(slot, value);
  end  
end

function RenderNode:SetShow(isshow)
  if self.isshow_ ~= isshow then
    self.isshow_ = isshow;
    if isshow then
      self.render_:SetRenderProperty(Engine.RP_SHOW);
    else
      self.render_:EraseRenderProperty(Engine.RP_SHOW);
    end  
  end
end

function RenderNode:SetShadowCaster(iscaster_)
  if self.iscaster_ ~= iscaster_ then
    self.iscaster_ = iscaster_;
    if iscaster_ then
      self.render_:SetRenderProperty(Engine.RP_SHADOW_CASTER);
    else
      self.render_:EraseRenderProperty(Engine.RP_SHADOW_CASTER);
    end
  end
end


function RenderNode:SetCull(iscull)
  if iscull then
    self.render_:SetRenderProperty(Engine.RP_CULL);
  else
    self.render_:EraseRenderProperty(Engine.RP_CULL);
  end  
end

function RenderNode:SetBindBox(aabb)
  self.render_:SetBindBox(aabb);
end

function RenderNode:GetBindBox()
  return self.render_:GetBindBox();
end

function RenderNode:SetSequence(s)
  self.render_:SetSequence(s);
end

function RenderNode:GetSequence()
  return self.render_:GetSequence();
end

function RenderNode:GetVertexStream()
  return self.render_:GetVertexStream();
end

function RenderNode:GetIndexStream()
  return self.render_:GetIndexStream();
end

function RenderNode:SetKeepSource(iskeepsource)
  self.render_:SetKeepSource(iskeepsource);
end

function RenderNode:IsRenderProperty(p)
  return self.render_:isRenderProperty(p);
end

return RenderNode