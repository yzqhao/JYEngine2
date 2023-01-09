local defined = require "window.editor.system.defined"
local imguifunction = require "imguifunction"
local Engine = require "Engine"
local Math = require "Math"
local Core = require "MeteorCore"


local ImGui = imguifunction.ImGui;


local utility = {}

function utility.Vector1(vec, name, speed, hash, min, max, hintname)
  ImGui.Text(name);
  if hintname then
    utility.Hint(hintname);
  end
  ImGui.SameLine();
  hash = hash or "vector1";
  speed = speed or 0.01;
  local noInput = false;
  if min == nil and  max == nil then
    noInput = true;
  end
  min = min or 0.0;
  max = max or 0.0;
  if type(vec) == "userdata" then
    local ischange, newdata = imguifunction.ImGui.DragFloat("##" .. name ..hash, vec.x, speed,min,max);
    if noInput == false then
      if newdata < min then
        newdata = min
      elseif newdata > max then
        newdata = max
      end
    end
    return ischange, newdata
  else
    local ischange, newdata =  imguifunction.ImGui.DragFloat("##" .. name ..hash, vec, speed,min,max);
    if noInput == false then
      if newdata < min then
        newdata = min
      elseif newdata > max then
        newdata = max
      end
    end
    return ischange, newdata
  end
end

function utility.Vector1_dec(vec, name, speed, hash, min,max)
  ImGui.Text(name);
  ImGui.SameLine();
  hash = hash or "vector1";
  speed = speed or 0.01;
  local noInput = false;
  if min == nil and  max == nil then
    noInput = true;
  end
  min = min or 0.0;
  max = max or 0.0;
  if type(vec) == "userdata" then
    local ischange, newdata = imguifunction.ImGui.DragFloat_dec("##" .. name ..hash, vec.x, speed,min,max,"%.4f");
    if noInput == false then
      if newdata < min then
        newdata = min
      elseif newdata > max then
        newdata = max
      end
    end
    return ischange, newdata
  else
    local ischange, newdata =  imguifunction.ImGui.DragFloat_dec("##" .. name ..hash, vec, speed,min,max,"%.4f");
    if noInput == false then
      if newdata < min then
        newdata = min
      elseif newdata > max then
        newdata = max
      end
    end
    return ischange, newdata
  end
end

function utility.IVector1(vec,name,speed,min,max,hash)
  ImGui.Text(name);
  ImGui.SameLine();
  hash = hash or "vector1";
  speed = speed or 1.0;
  min = min or -1000;
  max = max or 1000;
  local ischange,newdata;
  if type(vec) == "userdata" then
    ischange, newdata = imguifunction.ImGui.DragInt("##" .. name .. hash, vec.x,speed,min,max);
  else
    ischange, newdata =  imguifunction.ImGui.DragInt("##" .. name .. hash, vec, speed,min,max);
  end
  
  if newdata < min then
    newdata = min
  elseif newdata > max then
    newdata = max
  end
  return ischange, newdata
end

function utility.Vector2(vec, name, speed, hash,min,max, hintname)
  hash = hash or (vec:GetObjectID());
  ImGui.Text(name);
  if hintname then
    utility.Hint(hintname);
  end
  ImGui.SameLine();
  speed = speed or 0.01;
  if min and max then
    return imguifunction.ImGui.DragFloat2("##" .. name .. hash, vec, speed,min,max);
  else
    return imguifunction.ImGui.DragFloat2("##" .. name .. hash, vec, speed);
  end  
end

function utility.IVector2(vec,name,speed,min,max,hash)
  hash = hash or (vec:GetObjectID());
  ImGui.Text(name);
  ImGui.SameLine();
  speed = speed or 1.0;
  min = min or 0.0;
  max = max or 0.0;
  return imguifunction.ImGui.DragInt2("##" .. name .. hash,vec,speed,min,max);
end


function utility.Vector3(vec, name, speed, hash, min, max, hintname)
  hash = hash or (vec:GetObjectID());
  ImGui.Text(name);
  if hintname then
    utility.Hint(hintname);
  end
  ImGui.SameLine();
  speed = speed or 0.01;

  if min and max then
    return imguifunction.ImGui.DragFloat3("##" .. name ..hash, vec, speed, min, max, "%.3f", 1.0);
  else    
    return imguifunction.ImGui.DragFloat3("##" .. name ..hash, vec, speed);
  end

  
end

function utility.Vector3_dec(vec, name, speed, hash, min, max)
  hash = hash or (vec:GetObjectID());
  ImGui.Text(name);
  ImGui.SameLine();
  speed = speed or 0.01;

  if min and max then
    return imguifunction.ImGui.DragFloat3_dec("##" .. name ..hash, vec, speed, min, max, "%.4f", 1.0);
  else    
    return imguifunction.ImGui.DragFloat3_dec("##" .. name ..hash, vec, speed, "%.4f");
  end

end

function utility.EularAngleInspect(vec, name, speed, hash)
  hash = hash or (vec:GetObjectID());
  ImGui.Text(name);
  ImGui.SameLine();
  speed = speed or 0.01;
  return imguifunction.ImGui.DragFloat3("##".. name .. hash, vec/math.pi*180, speed); --直接设置trans.LocalEularAngle/math.pi*180会导致hash变化使得反射面板拖拽失败
end

function utility.Vector4(vec, name, speed, hash, hintname)
  hash = hash or (vec:GetObjectID());
  ImGui.Text(name);
  if hintname then
    utility.Hint(hintname);
  end
  ImGui.SameLine();
  speed = speed or 0.01;
  return imguifunction.ImGui.DragFloat4("##".. name .. hash, vec, speed);
end

function utility.Vector4_dec(vec, name, speed, hash, min, max, hintname)
  hash = hash or (vec:GetObjectID());
  ImGui.Text(name);
  if hintname then
    utility.Hint(hintname);
  end
  ImGui.SameLine();
  speed = speed or 0.01;
  return imguifunction.ImGui.DragFloat4("##".. name .. hash, vec, speed, min, max, "%.3f", 1.0);
end

function utility.Quaternion(vec, name, speed, hash)
  hash = hash or (vec:GetObjectID());
  ImGui.Text(name);
  ImGui.SameLine();
  speed = speed or 0.005;
  local ischange, nd = imguifunction.ImGui.DragFloat4("##".. name ..hash, Math.vector4(vec.x, vec.y, vec.z, vec.mw), speed);
  local q = Math.Quaternion(nd.x, nd.y, nd.z, nd.mw);
  q:NormalizeSelf();
  return ischange, q;
end


function utility.ErrorView(name, text, callback, moreHint)
  ImGui.OpenPopup(name);
  if ImGui.BeginPopupModal(name, true, ImGui.ImGuiWindowFlags_AlwaysAutoResize + ImGui.ImGuiWindowFlags_NoTitleBar) then
    ImGui.Text(text);
    ImGui.Separator();
    ImGui.Text(moreHint or "");
    ImGui.Dummy(Math.Vec2(100,0));
    ImGui.SameLine();
    if ImGui.Button("ok", Math.Vec2(120,0)) then
      callback();
      ImGui.CloseCurrentPopup();
    end
    ImGui.EndPopup();
  end
end

function utility.FileSelecter(name, path, btnName, filter, hash, callback, colation, uniformname, textype,extraPath)
  local type_rtti = nil;
  local filterNames = nil;
  if type(filter) == "userdata"
    and filter:isType(Engine.TextureEntity:RTTI()) then
      type_rtti = filter;
      if textype == Engine.TextureEntity.TT_TEXTURECUBE then
        filterNames = {defined.FileTypeList.Hdr}
      else
        filterNames = { unpack(defined.FileTypeList.Texture) };
        table.insert(filterNames, defined.FileTypeList.FrameAnimation);
      end
  else
    filterNames = filter;
  end
  
  local size = Engine.Engine:Instance():GetViewport();
  local btnSize = Math.Vec2(size.z / 8.0, size.w / 45.0);
  local btnSize2 = Math.Vec2(size.z / 8.0, size.w / 45.0);
  
  if uniformname then
    ImGui.Text(uniformname); --反射面板调用 看到uniform字符串
    utility.Hint(name); --tips是说明
  else
    ImGui.Text(name);  --其他地方调用
  end
  ImGui.SameLine();
  
  local style = ImGui.GetStyle();
  local gray = style:GetColor(ImGui.ImGuiCol_FrameBg);
  ImGui.PushStyleColor(ImGui.ImGuiCol_Button, gray);
  ImGui.PushStyleColor(ImGui.ImGuiCol_ButtonHovered, gray);
  ImGui.PushStyleColor(ImGui.ImGuiCol_ButtonActive, gray);
  local istouch = ImGui.Button(path.."##"..name, btnSize2);
  ImGui.PopStyleColor(3);
  
  ImGui.SameLine();
  if ImGui.Button(btnName .. "##" .. hash, btnSize) then
    GlobalSeclectWindow:Open(
      filterNames,
      callback,
      true,
      extraPath,
      nil,
      nil,
      colation
    );
    GlobalSeclectWindow:SetRttiFilter(type_rtti);
  end
  return istouch;
end

function utility.Hint(str)
  if ImGui.IsItemHovered() then
    ImGui.BeginTooltip();
    ImGui.PushTextWrapPos(ImGui.GetFontSize() * 15.0)
    ImGui.TextUnformatted(str);
    ImGui.PopTextWrapPos();
    ImGui.EndTooltip();
  end
end

return utility;