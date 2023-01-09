local imguifunction = require "imguifunction"
local Math = require "Math"
local Engine = require "Engine"
local Core = require "MeteorCore"
local Object = require "MeteorCore.core.Object"

local filelfs = require "window.editor.widget.assets.fileutility"
local defined = require "window.editor.system.defined"
local ImGui = imguifunction.ImGui;
local panelutility = require "window.editor.widget.inspector.panels.utility"

local EditorSystem = require "window.editor.system.editsystem"

local selectwindow = class("selectwindow", Object)

function selectwindow:ctor()
  self.open = false;
  self.namefilter = "";
  self.selectCallBack = {};
  self.closeCallBack = nil;
  self.typefilter = {};
  self.br = "";
  self.window_flags = ImGui.ImGuiWindowFlags_MenuBar;
  self.showFolder = false;
  self.titleName = "Select";
  self.targetPath  = nil;
  self.type_rtti = nil;
  self.deviceColation = nil;
  
  self.serchFolder = {};
  self.warningText = nil;
  self.extraFileTree = {};
  self.specialExtraFile = {};
  self.hasInit = false;
end

function selectwindow:Open(typefilter,selectCallBack,showFolder,extraPath,closeCallBack, titleName,deviceColation)
  self.titleName = titleName or "Select";
  self.namefilter = "";
  self.typefilter = typefilter;
  self.selectCallBack = selectCallBack;
  self.closeCallBack = closeCallBack;
  self.open = true;
  self.showFolder = false;
  --自定义选择资源的目标路径
  self.targetPath = SystemDirectories["scene_asset"];
  self.type_rtti = nil;
  self.extraPath = extraPath;
  self.specialExtraFile = {};
  self.hasInit = false;
  self.deviceColation = deviceColation;
    if showFolder then
    self.showFolder = showFolder;
    self.filetree = filelfs:GetDirectoryInFileTree(self.targetPath);
  end
end

function selectwindow:SetTargetPath(targetPath)
  self.targetPath = targetPath or SystemDirectories["scene_asset"];
  if self.showFolder then
    self.filetree = filelfs:GetDirectoryInFileTree(self.targetPath);
  end
end
 
function selectwindow:AddSpecialExtraFile(file)
  self.specialExtraFile = file;
end

function selectwindow:SetRttiFilter(type_rtti)
  self.type_rtti  = type_rtti;
end

function selectwindow:OnGui(timespan)
    if self.open then
      local b1,close = ImGui.Begin(self.titleName,true, self.window_flags);
      if close == false then --or ImGui.IsWindowFocused() == false then
        self:Close();
      end
      local inputTextFlags = ImGui.ImGuiInputTextFlags_EnterReturnsTrue + ImGui.ImGuiInputTextFlags_AutoSelectAll;
      local bc, text = ImGui.InputText(" ",self.namefilter,64,inputTextFlags);
      self.br = bc;
      if self.br then
        self.namefilter = text;
      end
     
      ImGui.Separator();
      if self.showFolder then
        self:ShowFolderFilesByFilter(self.filetree, self.selectCallBack);
      else
        self:ShowFilesByFilter(FileTree, self.typefilter, self.namefilter, self.selectCallBack);
      end

      if self.warningText then
        panelutility.ErrorView("Texture error", self.warningText , function() self.warningText = nil; end, "Max size is 2048 x 2048");
      end

      ImGui.End();
    else
      self.hasInit = false;
    end

end

function selectwindow:ShowFilesByFilter(filetree, typefilter, namefilter, callback)
  for key, value in pairs(filetree) do
    if type(value) == "table" then
      self:ShowFilesByFilter(value, typefilter, namefilter, callback)
    else
      if filelfs:Contains(typefilter, filelfs:getExtension(key)) and string.find(key,namefilter) ~= nil then
        if ImGui.Selectable(filelfs:getFileNameWithPosfix(key), false, ImGui.ImGuiSelectableFlags_AllowDoubleClick, imguifunction.ImVec2(0,0)) then
          if ImGui.IsMouseDoubleClicked(0) then
            if callback~= nil then
              callback(filelfs:ConverToRelativePath(key));
            end
            self:Close();
          end
        end
      end
    end
  end
end

function selectwindow:ShowFolderFilesByFilter(filetree, callback)
  --把符合搜索条件的文件夹都放到一个table里

  local fileresourcetree = self:AddDevResourceList(filetree);
  fileresourcetree = self:_AddExtrafileList(fileresourcetree);
  fileresourcetree = self:_AddSpecialExtrafileList(fileresourcetree);
  if self.br then
    self.serchFolder = {};
    self:serchFoder(fileresourcetree);
  end
  if self.namefilter == "" then
    self:showdirectory(fileresourcetree, callback);
  else
    self:showSerchFoders(fileresourcetree, callback);
  end
end
function selectwindow:serch(filetree)
  if filetree == nil then
    return;
  end
  for j = 2, #filetree do
    local filename = nil;
    local childtable = nil;
    if type(filetree[j]) == "table" then
      filename = filetree[j][1];
      childtable = filetree[j];
    end
    
    if filename~=nil and string.find(string.lower(filename),string.lower(self.namefilter)) ~= nil then
      local filetype = filelfs:getExtension(filename);
      if filetype==nil then
        self.serchFolder[filename] = filetree;
      end
    end
    self:serch(childtable);
  end
end

function selectwindow:serchFoder(filetree)
    if self.namefilter ~= "" then
      self:serch(filetree);
    end  
end

function selectwindow:showSerchFoders(filetree,callback)
  for key, value in pairs(self.serchFolder) do      
   self:showdirectory(value,callback,key);
  end  
end

function selectwindow:showdirectory(filetree, callback, foldername)
  local showOneFolder = false;
  if foldername ~= nil and foldername ~= "" then
    showOneFolder = true;
  end
  for i = 2, #filetree do
    local filename = nil;
    local childtable = nil;
    if type(filetree[i]) == "table" then
      filename = filetree[i][1];
      childtable = filetree[i];
    else
      filename = filetree[i];

    end
    --
    local find = false;
    if showOneFolder == true and filename == foldername then
      find = true;
    elseif showOneFolder == false and string.find(string.lower(filename),string.lower(self.namefilter)) ~= nil then
      find = true;
    end
   
    if find then
      local filetype = filelfs:getExtension(filename);
      if filetype == nil or string.lower(filetype) == "fbx"
              or string.lower(filetype) == "obj"
              or self:IsTargetType(filetype)
              or filelfs:Contains(self.specialExtraFile, filename)
      then
        imguifunction.ImGui.PushID(filename);
        imguifunction.ImGui.AlignTextToFramePadding();
        local is_selected = false;  --标识选中状态
        local selected = EditorSystem:GetSelected();
        if selected and selected == filename then
          is_selected = true;
        end

        local nodeName = filelfs:getFileNameWithPosfix(filename);
        nodeName = self:_GetInternalName(nodeName);
        local nodeFlags = defined.TreeNodeFlag + imguifunction.ImGui.ImGuiTreeNodeFlags_NoAutoOpenOnLog;

        if is_selected then
          nodeFlags  = nodeFlags + imguifunction.ImGui.ImGuiTreeNodeFlags_Selected;
        end

        local no_children = true;
        if childtable ~= nil then
          no_children = false;
        end

        if no_children == true then --没有子Node,则创建叶子节点
          nodeFlags = nodeFlags + imguifunction.ImGui.ImGuiTreeNodeFlags_Leaf;
        end
        imguifunction.ImGui.AlignTextToFramePadding();
        local opened = imguifunction.ImGui.TreeNodeEx(nodeName,nodeFlags);

        if imguifunction.ImGui.IsItemHovered() and imguifunction.ImGui.IsMouseDragging() == false then
          if ImGui.IsMouseDoubleClicked(0) then
            local devresourceType = self:GetDevResourceType(filename)
            if self:IsTargetType(filetype) or devresourceType or filelfs:Contains(self.specialExtraFile, filename) then
              if callback~= nil then
                local extension = filelfs:getExtension(filename);
                if extension ~= nil then
                  extension = string.lower(extension);
                end
                extension = extension or devresourceType;
                local relativeHead = {};
                table.insert(relativeHead, "proj:")
                if self.extraPath ~= nil then
                  if type(self.extraPath) == "table" then
                    for i = 1, #self.extraPath do
                      table.insert(relativeHead, self.extraPath[i]);
                    end
                  elseif type(self.extraPath) == "string" then
                    table.insert(relativeHead, self.extraPath);
                  end
                end
                table.insert(relativeHead, self.targetPath);
                local dstpath = filelfs:GetRelativePath(filename, relativeHead);
                local entity = self:CreateEntity(dstpath,extension) or dstpath;
                if not self.warningText then --创建出来的纹理有问题不要回调
                  callback(dstpath,extension, entity);
                end
              end
              if not self.warningText then --创建出来的纹理有问题不要关闭selectwindow
                self:Close();
                if self.closeCallBack ~= nil then
                  self.closeCallBack(filelfs:ConverToRelativePath(filename,self.targetPath));
                end
              end
            end
          end
        end
        if opened == true then
          if no_children == false then  --迭代子节点
            self:showdirectory(childtable, callback);
          end
          imguifunction.ImGui.TreePop();
        end
        imguifunction.ImGui.PopID();
      end
    end
  end
end

function selectwindow:Close()
  self.open = false;
  self.namefilter = "";
  self.selectCallBack = {};
  self.typefilter = {};
  self.serchFolder = {};
  self.hasInit = false;
  self.extraFileTree = {};
  self.specialExtraFile = {};
end

function selectwindow:IsTargetType(thetype)
  if thetype == nil then
    return false;
  end
  for _, v in ipairs(self.typefilter) do
		if string.lower(v) == string.lower(thetype) then
			return true
		end
	end
  
  return false;
end

function selectwindow:GetTitleName()
  return self.titleName;
end

function selectwindow:_AddExtrafileList(filetree)
  if self.extraPath == nil then
    return filetree;
  end

  if not self.hasInit then
    self.extraFileTree = {};
    for i = 1, #filetree do
      table.insert(self.extraFileTree, filetree[i]);
    end
    if type(self.extraPath) ~= "table" then
      self.extraPath = {self.extraPath};
    end
    for i = 1, #self.extraPath do
      if self.extraPath[i] ~= SystemDirectories["scene_library"] then
        local allMeshs = filelfs:GetFilesInDir(self.extraPath[i], true, self.typefilter);
        local tempFiles = selectwindow:_SplitFileList(allMeshs, self.extraPath[i]);
        table.insert(tempFiles, 1, self.extraPath[i]);
        tempFiles = { tempFiles };
        for i = 1, #tempFiles do
          table.insert(self.extraFileTree, tempFiles[i]);
        end
      end
    end
    self.hasInit = true;
  end

  return self.extraFileTree;
end

function selectwindow:_AddSpecialExtrafileList(filetree)
  if next(self.specialExtraFile) == nil then
    return filetree;
  end
  local specialExtraFileTree = {};
  table.insert(specialExtraFileTree,filetree[1]);
  for i = 1, #self.specialExtraFile do
    table.insert(specialExtraFileTree, self.specialExtraFile[i]);
  end
  for i = 2, #filetree do
    table.insert(specialExtraFileTree,filetree[i]);
  end
  return specialExtraFileTree;
end

function selectwindow:_SplitFileList(allmeshs, path)
  local subFiles = {};
  for k, v in pairs(allmeshs) do
    if type(v) == "table" then
      local str1 = path .. "/" .. k;
      str1 = string.gsub(str1, '//', '/');
      local tempFiles = selectwindow:_SplitFileList(v, str1);
      table.insert(tempFiles, 1, str1);
      table.insert(subFiles, tempFiles);
    else
      local str1 = path .. "/" .. v;
      str1 = string.gsub(str1, '//', '/');
      table.insert(subFiles, str1);
    end
  end
  table.sort(subFiles, 
    function(path1, path2)
      if type(path1) == type(path2) then
        if type(path1) == "table" then
          return path1[1] < path2[1];
        else
          return path1 < path2;
        end
      else
        return type(path2) == "table";
      end
    end
  );
  return subFiles; 
end

function selectwindow:removeByValue(array, value, removeadll)
    local deleteNum,i,max=0,1,#array
    while i<=max do
        if array[i] == value then
            table.remove(array,i);
            deleteNum = deleteNum+1 ;
            i = i-1;
            max = max-1;
            if not removeadll then break end
        end
        i= i+1;
    end
    return deleteNum --    返回删除次数
end


function selectwindow:_CheckValueInTable(table,value)
  if table then
    for k,v in ipairs(table) do
      if v == value then
        return true;
      end
    end
  end
  return false;
end

function selectwindow:AddDevResourceList(filetree)
  if self.type_rtti == nil then
    return filetree
  end
  
  local typename = self.type_rtti:GetTypeName();
  local deviceresources  = {};
  local templist = apolloengine.DeviceResource:GetDeviceResources(self.type_rtti);
  
  if templist == nil then
    return filetree
  end

  table.insert(deviceresources,1,"Device"..typename)
  for i = 1,#templist do
    if not self:_CheckValueInTable(self.deviceColation, templist[i]) then
      table.insert(deviceresources,templist[i])
    end
  end
  
  local devicefiletree = {};
  table.insert(devicefiletree,filetree[1]);
  table.insert(devicefiletree,deviceresources);
  for i = 2, #filetree do
    table.insert(devicefiletree,filetree[i]);
  end
  return devicefiletree;
end

function selectwindow:GetDevResourceType(filename)
  if filename == nil or self.type_rtti == nil then
    return nil;
  end
  local resourcelist = Engine.DeviceResource:GetDeviceResources(self.type_rtti)
  if resourcelist == nil then
    return nil
  end
  for key,value in pairs(resourcelist) do
    if value == filename then
      return defined.RHIDefine.ReFS;
    end
  end
  return nil;
end

function selectwindow:CreateEntity(targetPath, extension)
  if self.type_rtti == nil then
    return nil;
  end
  local entity = nil;
  if self.type_rtti:isType(Engine.TextureEntity:RTTI()) then
    if extension == defined.RHIDefine.ReFS then
      entity= Engine.TextureEntity();
      entity:PushMetadata(Engine.TextureReferenceMetadata(targetPath));
      entity:CreateResource();
    elseif extension == defined.RHIDefine.Fbo then
      entity = Engine.RenderTargetEntity();
      entity:PushMetadata(Engine.RenderTargetFileMetadata(targetPath));
      entity:CreateResource();
    elseif extension == defined.RHIDefine.HDR then
      entity = self:CreateCubemapEntity(targetPath)
    elseif extension == defined.FileTypeList.FrameAnimation then
      entity = nil;
    else
      entity= Engine.TextureEntity();

      local metaPath = filelfs:GetTextureMeta(targetPath);
      local rMetaPath = filelfs:ConverToRelativePath(metaPath);
      entity:PushMetadata(Engine.TextureDescribeFileMetadata(rMetaPath));
      entity:SetKeepSource(true);
      entity:CreateResource();

      local texSize = entity:GetSize();
      if texSize.x > 2048 or texSize.y > 2048 then
        self.warningText = "texture size "..tostring(texSize.x).." x "..tostring(texSize.y).." is too large";
      end
    end
  end
  return entity;
end

function selectwindow:CreateCubemapEntity(targetPath)
  local meta = filelfs:GetMetaData(targetPath)
  if meta == nil or meta.fileType == nil then
    return nil
  end
  if meta.fileType ~= "CubemapSource" then
    return nil
  end
  local prefix = "out"
  local faces = {"posz", "negz", "posy", "negy", "negx", "posx"}
  local sizes = {"256x256", "128x128", "64x64", "32x32", "16x16", "8x8", "4x4", "2x2", "1x1"}
  local textureTypes = {
    Engine.TextureEntity.TT_TEXTURECUBE_FRONT,
    Engine.TextureEntity.TT_TEXTURECUBE_BACK,
    Engine.TextureEntity.TT_TEXTURECUBE_TOP,
    Engine.TextureEntity.TT_TEXTURECUBE_BOTTOM,
    Engine.TextureEntity.TT_TEXTURECUBE_LEFT,
    Engine.TextureEntity.TT_TEXTURECUBE_RIGHT,
  }
  local mipMaps = {0,1,2,3,4,5,6, 7, 8}
  local metadatas = {}
  if meta.convolveType == 0 then
    for i = 1, #faces do
      local facePath = string.format("%s/%s_%s.tga", meta.targetPath, prefix, faces[i])
      local metadata = Engine.TextureFileMetadata(textureTypes[i], mipMaps[1], facePath)
      table.insert(metadatas, metadata)
    end
  elseif meta.convolveType == 1 then
    for j = 1, #sizes do
      for i = 1, #faces do
        local facePath = string.format("%s/%s_%s_%d_%s.tga", meta.targetPath, prefix, faces[i], mipMaps[j], sizes[j])
        local metadata = Engine.TextureFileMetadata(textureTypes[i], mipMaps[j], facePath)
        table.insert(metadatas, metadata)
      end
    end
  elseif meta.convolveType == 2 then
    for i = 1, #faces do
      local facePath = string.format("%s/%s_%s.tga", meta.targetPath, prefix, faces[i])
      local metadata = Engine.TextureFileMetadata(textureTypes[i], mipMaps[1], facePath)
      table.insert(metadatas, metadata)
    end
  else
    ERROR("unsupported convolveType: " .. tostring(meta.convolveType))
    return nil
  end
  local tex = Engine.TextureEntity()
  for i = 1, #metadatas do
    tex:PushMetadata(metadatas[i])
  end
  tex:CreateResource()
  return tex
end

function selectwindow:_GetInternalName(nodeName)
  local res = nodeName;
  local internalDir = defined.Internal;
  for i = 1, #internalDir do
    if nodeName == internalDir[i] then
      res = "Internal"
    end
  end
  return res;
end


return selectwindow;