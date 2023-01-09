--[[
    desc: cclass GOBject wrap
    author:{hjy}
    time:2022-09-28 18:59:35
]]

require("functions")

local Object = require("MeteorCore.core.Object")
local GameObject = class("GameObject", Object)

function GameObject:ctor(scene, name)
    GameObject.super.ctor()
    self.name_ = name or "default_node"
    self.scene_ = scene
    self.components_map_={}
    self.Internal_ = self.scene_:GetInternal():CreateObject(self.name_);
end

function GameObject:GetInternal()
    return self.Internal_
end

function GameObject:AddComponent(component_type)
    local component_instance = component_type.new(self)
    if self.components_map_[component_type]==nil then
        self.components_map_[component_type]={}
    end
    table.insert(self.components_map_[component_type], component_instance)
    return component_instance
end

function GameObject:GetComponent(component_type)
    if self.components_map_[component_type] and #self.components_map_[component_type]>0 then
        return self.components_map_[component_type][1]
    end
    return nil
end

function GameObject:GetComponents(component_type)
    local return_components={}
    for key, components in pairs(self.components_map_) do
        if key==component_type or is_sub_class_of(key, component_type) then
            --print("GetComponents check:" .. table_tostring(components))
            combine_list(return_components, components)
        end
    end
    return return_components
end

return GameObject