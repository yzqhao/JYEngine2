--[[
    desc: cclass Scene wrap
    author:{hjy}
    time:2022-09-28 19:00:25
]]

local Math = require("Math")
local Engine = require("Engine")
local Object = require("MeteorCore.core.Object")
local GameObject = require("framework.engine.GameObject")
local RenderComponent = require("framework.engine.RenderComponent")
local CameraComponent = require("framework.engine.CameraComponent")
local SceneSequence = require("framework.engine.SceneSequence")

if _EDITOR then
  function Engine.Scene:Init()
    self:SetSequence(SceneSequence.HIGH)
    self:CreateDefaultRenderTarget(Math.IntVec2(128,128));
  end

  function Engine.Scene:CreateGrid()
    local cellLength = 1.0;
    local cellCnt = 100;
    local material = "comm:material/editor/unlight_color";
    local sidelength = cellLength;
    local size = cellCnt;
    
    local gridNode = self:CreateGenericNode("Grid");
    gridNode:SetLayer(Engine.GraphicDefine.MC_MASK_EDITOR_SCENE_LAYER);
    
    local renderComponent = gridNode:CreateRenderComponent();
    renderComponent:EraseRenderProperty(Engine.GraphicDefine.RP_CULL);
    renderComponent:SetRenderProperty(Engine.GraphicDefine.RP_IGNORE_PICK);
    --renderComponent:SetBindBox(Math.Aabbox3d(Math.Vec3(0,0,0), Math.Vec3(0,0,0)));--让grid无法被拾取
    
    local vertexStream = Engine.VertexStream();
    local indexStream = Engine.IndicesStream();
    vertexStream:SetVertexType(Engine.RHIDefine.ATTRIBUTE_POSITION,
      Engine.RHIDefine.DT_FLOAT,
      Engine.RHIDefine.DT_HALF_FLOAT,
      4);
    indexStream:SetIndicesType(
      Engine.IndicesBufferEntity.IT_UINT16);
    
    vertexStream:ReserveBuffer(4 * size);--预先分配顶点的内存  4*n
    indexStream:ReserveBuffer(4 * size + 4);--4*n+4
    local val=size / 2;

    for i=-val, val do   
      vertexStream:PushVertexData(
        Engine.RHIDefine.ATTRIBUTE_POSITION,
        Math.Vec4(i*sidelength,0,-val*sidelength,1):GetPtr());
      vertexStream:PushVertexData(
        Engine.RHIDefine.ATTRIBUTE_POSITION,
        Math.Vec4(i*sidelength,0,val*sidelength,1):GetPtr());
    end

    for i = 1-val, (val-1) do  
      vertexStream:PushVertexData(
        Engine.RHIDefine.ATTRIBUTE_POSITION,
        Math.Vec4(-val*sidelength,0,i*sidelength,1):GetPtr());
      vertexStream:PushVertexData(
        Engine.RHIDefine.ATTRIBUTE_POSITION,
        Math.Vec4(val*sidelength,0,i*sidelength,1):GetPtr());
    end
   
    for i=0,4*size-1 do  
      indexStream:PushIndicesData(i);
    end
    indexStream:PushIndicesData(0);
    indexStream:PushIndicesData(2*size);
    indexStream:PushIndicesData(1);
    indexStream:PushIndicesData(2*size+1); 
    
    local mat = Engine.MaterialEntity()
    mat:PushMetadata(
      Engine.MaterialMetadata(material));
    mat:CreateResource();
    renderComponent:AddMaterialEntity(mat);

    local vmeta = Engine.ReferenceVertexMetadata(Engine.RHIDefine.MU_STATIC, vertexStream)
    local imeta = Engine.ReferenceIndicesMetadata(Engine.RHIDefine.MU_STATIC, indexStream)
    renderComponent:PushMetadata(Engine.RenderObjectMeshMetadate( 
      Engine.RHIDefine.RM_LINES,
      vmeta,
      imeta))
      renderComponent:CreateResource();
    
    local gridColor = Engine.IMaterialSystem:Instance():NewParameterSlot(Engine.RHIDefine.UNIFORM, "POINT_COLOR");
    
    if Engine.ProjectSetting:Instance():IsCurrentAdvancedShading() then
      renderComponent:SetParameter(gridColor, Math.Vec4(0.214,0.214,0.214,1.0));
    else
      renderComponent:SetParameter(gridColor, Math.Vec4(0.5,0.5,0.5,1.0));
    end
    
    return gridNode;
  end

  function Engine.Scene:CreateEditorCamera()  
    local size = Math.IntVec2(128,128);
    local near = 0.1;
    local far = 100;
    local pos = Math.Vec3(0,1,4);
    local lookat = Math.Vec3(0,0,0);
    local up = Math.Vec3(0,1,0);
    local EditorCameraName = "EditorCamera";
    
    local rendertarget = Engine.RenderTargetEntity();--创建一个FBO
    rendertarget:PushMetadata(--设置FBO格式
      Engine.RenderTargetMetadata(
        Engine.RHIDefine.TEXTURE_2D,
        Math.IntVec4(0,0,size.x,size.y),--视口大小
        size));--分辨率
    local depth = rendertarget:MakeTextureAttachment(Engine.RHIDefine.TA_DEPTH_STENCIL);--增加深度纹理
        depth:PushMetadata(
          Engine.DepthRenderBufferMetadata(
            size,
            Engine.RHIDefine.PF_DEPTH24_STENCIL8
          ));--增加深度
    local outputtexture = rendertarget:MakeTextureAttachment(Engine.RHIDefine.TA_COLOR_0);--增加color0纹理
    outputtexture:PushMetadata(--创建纹理
          Engine.TextureBufferMetadata(size));
    rendertarget:CreateResource();
    
    local luacamera3D = self:CreateGenericNode("EditorCamera");
    local cameraComponent = luacamera3D:CreateCameraComponent();
    local camera3D = luacamera3D;
    camera3D:SetLayer(Engine.GraphicDefine.MC_MASK_EDITOR_SCENE_LAYER);
    --camera3D:SetName(EditorCameraName);  --给一个默认的名字

    cameraComponent:ChangeResolution(size);
    cameraComponent:SetLayerMaskEverything();--导演的视角 有什么画什么
    cameraComponent:CreatePerspectiveProjection(cameraComponent:GetFov(), size.x/size.y, near,far);
    --cameraComponent:CreateRealCameraProjection(near,far);
    cameraComponent:LookAt(pos, lookat, up);
    cameraComponent:AttachRenderTarget(rendertarget);
    cameraComponent:Recalculate();
    cameraComponent:Activate();

    if Engine.ProjectSetting:Instance():IsCurrentAdvancedShading() then
      cameraComponent:SetClearColor(Math.FLinearColor(0.032,0.032,0.032,1.0));
    else
      cameraComponent:SetClearColor(Math.FLinearColor(0.2, 0.2, 0.2, 1.0));
    end
    
    return luacamera3D;
  end
end

function Engine.Scene:CreateGenericNode(nname)
  local node = self:CreateObject(nname);
  local trans = node:CreateTransformComponent();
  trans:SetLocalPosition(Math.Vec3(0.0,0.0,0.0));
  return node;
end
