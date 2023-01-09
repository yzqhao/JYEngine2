local Engine = require "Engine"
local Core = require "Core"

local hierarchyresource = {}

local newIcon = {
    path = "comm:documents/icons/new.png",
    tip = "New object",
    icon = nil
}
hierarchyresource.newIcon = newIcon

local layerIcon = {
    path = "comm:documents/icons/layer.png",
    tip = "Layer",
    icon = nil
}
hierarchyresource.layerIcon = layerIcon

local quadIcon = {
    path = "comm:documents/icons/quad.png",
    tip = "Quad",
    icon = nil
}
hierarchyresource.quadIcon = quadIcon

local lightIcon = {
    path = "comm:documents/icons/light.png",
    tip = "Light",
    icon = nil
}
hierarchyresource.lightIcon = lightIcon

local cameraIcon = {
    path = "comm:documents/icons/camera.png",
    tip = "Camera",
    icon = nil
}
hierarchyresource.cameraIcon = cameraIcon

local sphereIcon = {
    path = "comm:documents/icons/sphere.png",
    tip = "Sphere",
    icon = nil
}
hierarchyresource.sphereIcon = sphereIcon

local boxIcon = {
    path = "comm:documents/icons/box.png",
    tip = "Box",
    icon = nil
}
hierarchyresource.boxIcon = boxIcon

local particularIcon = {
    path = "comm:documents/icons/particular.png",
    tip = "Particular",
    icon = nil
}
hierarchyresource.particularIcon = particularIcon

local effectIcon = {
    path = "comm:documents/icons/effect.png",
    tip = "Effect",
    icon = nil
}
hierarchyresource.effectIcon = effectIcon

local faceIcon = {
    path = "comm:documents/icons/face.png",
    tip = "Face",
    icon = nil
}
hierarchyresource.faceIcon = faceIcon

function loadResources()
    local res = {
        newIcon, layerIcon, quadIcon, lightIcon, cameraIcon, effectIcon, boxIcon, particularIcon, sphereIcon, faceIcon
    }

    for key, value in pairs(res) do
        value.icon = Engine.TextureEntity()
        value.icon:PushMetadata(Engine.TextureFileMetadata(
            Engine.RHIDefine.TEXTURE_2D,
            Engine.RHIDefine.TU_STATIC,
            Engine.RHIDefine.PF_AUTO, 1, true, 0,
            Engine.RHIDefine.TW_CLAMP_TO_EDGE,
            Engine.RHIDefine.TW_CLAMP_TO_EDGE,
            Engine.RHIDefine.TF_LINEAR,
            Engine.RHIDefine.TF_LINEAR_MIPMAP_LINEAR,
            value.path))
        value.icon:CreateResource()
    end
end

function getNodeIcon(node)
    if node == nil or venuscore.isNil(node) then
        return effectIcon.icon
    end

    local names = node:GetComponentNames()
    local types = node:GetComponentTypes()

    for _, name in ipairs(names) do
        if name == "CameraComponent" then
            return cameraIcon.icon
        end
    end

    for _, name in ipairs(names) do
        if name == "LightComponent" then
            return lightIcon.icon
        end
    end

    for _, name in ipairs(names) do
        if name == "ParticleComponent" then
            return particularIcon.icon
        end
    end

    for idx, name in ipairs(names) do
        if name == "RenderComponent" then
            local comp = node:GetComponent(types[idx])
            if type(comp) == "userdata" then
                local mesh = comp.RenderObjectEntity:GetMeshName()
                if mesh ~= nil then
                    if string.find(mesh, "box") then
                        return boxIcon.icon
                    end
                    if string.find(mesh, "sphere") then
                        return sphereIcon.icon
                    end
                    if string.find(mesh, "face") then
                        return faceIcon.icon
                    end
                    if string.find(mesh, "head") then
                        return faceIcon.icon
                    end
                    if string.find(mesh, "quad") then
                        return quadIcon.icon
                    end
                end
                return boxIcon.icon
            end
        end
    end
    return effectIcon.icon
end

return hierarchyresource;