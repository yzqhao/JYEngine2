local lfs = require "lfs"
local venusjson = require "venusjson"
local Math = require("Math")
local imguifunction = require "imguifunction"
local Core = require "Core"
local defined = require "window.editor.system.defined"
local ImGui = imguifunction.ImGui;
local filelfs = {};

function filelfs:stringsplit(inputstr, sep, cover)
  cover = cover or function(str) return str end;
  if sep == nil then
    sep = "%s"
  end
  local t={} ; i=1
  for str in string.gmatch(inputstr, "([^"..sep.."]+)") do
    t[i] = cover(str)
    i = i + 1
  end
  return t
end
--get filename
function filelfs:getFileName(str)
  local idx = str:match(".+()%.%w+$")
  if(idx) then
    return str:sub(1, idx-1)
  else
    return str
  end
end

--get file postfix
function filelfs:getExtension(str)
  return str:match(".+%.(%w+)$")
end

function filelfs:getFileNameWithPosfix(path)
  local spiltarray = filelfs:stringsplit(path, "/", tostring);
  return spiltarray[#spiltarray];
end

function filelfs:SaveFileInfo(table, path)
  venusjson.SaveJsonObject(table,path);
end

function filelfs:ReadFileInfo(path)
  return venusjson.LaodJsonFile(path); --返回table
end

function filelfs:GetMetaData(path)
  path = Core.IFileSystem:Instance():PathAssembly(path)
  local metapath = path .. ".meta";
  return self:LoadMetaFile(metapath);
end

function filelfs:LoadMetaFile(metapath)
  local metadata = nil;
  if self:exists(metapath) then
    metadata = self:ReadFileInfo(metapath);
  end
  return metadata;
end

function filelfs:GetTextureMeta(path)
  if path == nil then
    LOGE("texture path is nil");
    return nil;
  end
  local metaPath = Core.IFileSystem:Instance():PathAssembly(path) .. ".meta";
  if not filelfs:exists(metaPath) then
    local textureInfo = table.deepcopy(defined.DefaultTextureSettings);
    textureInfo["TexturePath"] = self:ConverToRelativePath(path);
    venusjson.SaveJsonObject(textureInfo, metaPath);
  else
    local textureInfo = venusjson.LaodJsonFile(metaPath);
    local relativePath = self:ConverToRelativePath(path)
    if textureInfo["TexturePath"] == relativePath then
      
    else
        textureInfo["TexturePath"] = relativePath
        os.remove(metaPath);
        venusjson.SaveJsonObject(textureInfo, metaPath);
    end
  end
  return metaPath; 
end

function filelfs:GetTextureInfo(path)
  if path == nil then
    LOGE("texture path is nil");
    return nil;
  end
  local metaPath = Core.IFileSystem:Instance():PathAssembly(path) .. ".meta";
  local textureInfo = {};
  if filelfs:exists(metaPath) then
    textureInfo = venusjson.LaodJsonFile(metaPath);
  else
    textureInfo = table.deepcopy(defined.DefaultTextureSettings);
  end
  return textureInfo;
end

function filelfs:GetParentDirectory(path)
  local dir = path:match('.*/')
  return dir;
end

function filelfs:GetCurrentDirectory(path)
  local attr = lfs.attributes(path);
  if attr.mode == "directory" then
    return path;
  else
    return self:GetParentDirectory(path);
  end
end

function filelfs:copy(originpath, rootpath)
  originpath = Core.IFileSystem:Instance():PathAssembly(originpath);
  rootpath = Core.IFileSystem:Instance():PathAssembly(rootpath);
  local attr = lfs.attributes(originpath);
  local filename = filelfs:getFileNameWithPosfix(originpath);
  local targetpath = string.gsub(rootpath .. "/" .. filename,"//","/");
  if targetpath == originpath then
    LOG("target file is already exist, cancel copy");
  else
    if attr.mode ~= "directory" then
      local sourcefile = io.open(originpath,"rb");
      local destinationfile = io.open(targetpath, "wb");
      local len = sourcefile:seek("end");  --获取文件长度
      sourcefile:seek("set",0);--重新设置文件索引为0的位置
      local data = sourcefile:read(len);  --根据文件长度读取文件数据
      destinationfile:write(data);

      sourcefile:close();
      destinationfile:close();
    else
      lfs.mkdir(targetpath);
      for entry in lfs.dir(originpath) do
        if entry ~= "." and entry ~= ".." then
          local temp = originpath .. "/" .. entry;
          filelfs:copy(temp,targetpath);
        end
      end
    end
  end
end

function filelfs:exists(path)
  return lfs.attributes(path, "mode") ~= nil
end

function filelfs:CreateDirectory(path)
  local absolutepath = Core.IFileSystem:Instance():PathAssembly(path);
  if not filelfs:exists(absolutepath) then
    lfs.mkdir(absolutepath);
  end
  return path;
end

function filelfs:CreateDirectories(projectpath)
  local directories = {};
  for name, path in pairs(defined.FirstDirectories) do
    directories[name] = filelfs:CreateDirectory(projectpath .. path);
  end
  for name, path in pairs(defined.SecondDirectories) do
    directories[name] = filelfs:CreateDirectory(projectpath .. path);
  end
  return directories;
end

function filelfs:GetFilesInDir(path, recursive, ext)
  local res = {};
  local index = 1;
  local rootpath = Core.IFileSystem:Instance():PathAssembly(path); 
  for entry in lfs.dir(rootpath) do
    if entry ~= '.' and entry ~= '..' then
      local path = rootpath .. '/' .. entry;
      local attr = lfs.attributes(path);
      if attr ~= nil then
        if recursive and attr.mode == "directory" then
          res[entry] = self:GetFilesInDir(path, recursive, ext);
        else
          if not ext or self:Contains(ext,self:getExtension(entry)) then
            table.insert(res, entry)
          end
        end
      else
        LOGE("invalid path!");
      end
    end
  end
  return res;
end

function filelfs:AppendToFileTree(path, assetspath, filetree)
  local abpath = Core.IFileSystem:Instance():PathAssembly(path);
  abpath = string.gsub(abpath,"//","/");
  local assetspath = assetspath or Core.IFileSystem:Instance():PathAssembly(SystemDirectories["assets"]);
  local _, sEnd = string.find(abpath,assetspath,1,true);
  if sEnd ~= nil then
    local relativepath = string.sub(abpath,sEnd + 1);
    local temp = string.split(relativepath, "/");
    local temptree = filetree or FileTree;
    for i = 1, (#temp-1) do
      assetspath = assetspath .. "/" .. temp[i];
      assetspath = string.gsub(assetspath,"//","/");
      temptree = self:FindFile(assetspath,temptree);
      if temptree == nil then
        LOGE("file path do not exist");
        return false;
      end
    end
    local isDirectory = filelfs:getExtension(abpath) == nil;
    local insertpath = nil;
    if not isDirectory then
      local type = filelfs:getExtension(abpath);
      if type ~= nil and (string.lower(type) == "fbx" or string.lower(type) == "obj") then
        insertpath = self:GetFbxResource(abpath);
      else
        insertpath = abpath;
      end
    else
      insertpath = self:GetFileSystemTree(abpath);
    end
    local exist,index = self:DirectoryExistFile(temptree,abpath);
    if exist then
      temptree[index] = insertpath;
    else
      local directoryname = table.remove(temptree,1);
      table.insert(temptree,insertpath);
      self:SortTable(temptree);
      table.insert(temptree,1,directoryname);
    end
  end
  return true;
end

function filelfs:RemoveInFileTree(path, assetspath, filetree)
  local abpath = Core.IFileSystem:Instance():PathAssembly(path);
  abpath = string.gsub(abpath,"//","/");
  local assetspath = assetspath or Core.IFileSystem:Instance():PathAssembly(SystemDirectories["assets"]);
  local _, sEnd = string.find(abpath,assetspath,1,true);
  if sEnd ~= nil then
    local relativepath = string.sub(abpath,sEnd + 1);
    local temp = string.split(relativepath, "/");
    local temptree = filetree or FileTree;
    for i = 1, (#temp-1) do
      assetspath = assetspath .. "/" .. temp[i];
      assetspath = string.gsub(assetspath,"//","/");
      temptree = self:FindFile(assetspath,temptree);
      if temptree == nil then
        LOGE("file path do not exist");
        return false;
      end
    end
    local exist,index = self:DirectoryExistFile(temptree,abpath);
    if exist then
      table.remove(temptree,index);
    end
  end
  return true;
end

function filelfs:ShowFilesByCategory(filepath)
  local rootpath = Core.IFileSystem:Instance():PathAssembly(filepath);
  for entry in lfs.dir(rootpath) do
    if entry ~= '.' and entry ~= '..' then
      local path = rootpath .. '/' .. entry;
      local attr = lfs.attributes(path);
      if attr.mode == "directory" then
        if ImGui.BeginMenu(entry) then
          self:ShowFilesByCategory(path);
          ImGui.EndMenu();
        end
      else
        if ImGui.MenuItem(filelfs:getFileName(entry)) then
          LOG(path);
        end
      end
    end
  end
end

function filelfs:SearchFiles(path, ext, out)
  local res = out or {};
  local rootpath = Core.IFileSystem:Instance():PathAssembly(path);
  for entry in lfs.dir(rootpath) do
    if entry ~= '.' and entry ~= '..' then
      local fullPath = rootpath .. '/' .. entry;
      local attr = lfs.attributes(fullPath);
      if attr ~= nil then
        if attr.mode == "directory" then
          self:SearchFiles(fullPath, ext, res);
        elseif self:Contains(ext, self:getExtension(entry)) then
          table.insert(res, fullPath)
        end
      else
        LOGE("file error " .. fullPath..", entry "..entry)
      end
    end
  end
  return res;
end

function filelfs:ConverToRelativePath(absolutepath,targetPath)
  local relativepath = absolutepath;
  local replacestr = targetPath or defined.ProjectPath;
  local projectpath = Core.IFileSystem:Instance():PathAssembly(replacestr);
  local _, sEnd = string.find(absolutepath,projectpath,1,true);
  if sEnd ~= nil then
    relativepath = string.sub(absolutepath,sEnd + 1);
    relativepath = replacestr .. relativepath;
  else  --在尝试下Internal Path
    local internalDir = defined.Internal;
    for i = 1, #internalDir do
      local internalPath = Core.IFileSystem:Instance():PathAssembly(internalDir[i]);
      local _, sEnd = string.find(absolutepath,internalPath,1,true);
      if sEnd ~= nil then
        relativepath = string.sub(absolutepath,sEnd + 1);
        relativepath = internalDir[i] .. relativepath;
        break;
      end
    end
  end
  return relativepath;
end

function filelfs:GetRelativePath(absolutepath,targetPath)
  local relativepath = absolutepath;
  for i = 1, #targetPath do
    local relativeHead = targetPath[i];
    local replacestr = relativeHead or defined.ProjectPath;
    local projectpath = Core.IFileSystem:Instance():PathAssembly(replacestr);
    local i = 0;
    local _, sEnd = string.find(absolutepath,projectpath,1,true);
    if sEnd ~= nil then
      relativepath = string.sub(absolutepath,sEnd + 1);
      relativepath = replacestr .. relativepath;
      return relativepath;
    end
  end
  return relativepath;
end

function filelfs:GetCleanPathForDialog(path)
  local slashFirst = string.find(path, "/") == 1
  local temppath = Core.IFileSystem:Instance():PathAssembly(path);
  temppath = string.gsub(temppath,[[\]], [[/]]);
  local temp = string.split(temppath, [[/]]);
  while self:Contains(temp, "..") do
    local removelist = {};
    for i = 1, #temp do
      if temp[i] == ".." then
        table.insert(removelist,i);
        table.insert(removelist,i-1);
        break;
      end
    end
    for i = 1,#removelist do
      table.remove(temp,removelist[i]);
    end
  end
  temppath = "";
  for i = 1, #temp do
    temppath = temppath .. temp[i] .. [[/]];
  end
  local result = slashFirst and ("/" .. temppath) or temppath;
  return result
end

function filelfs:Contains(mtable, str)
  if mtable == nil then
    return false;
  end
  for i = 1,#mtable do
    if mtable[i] == str then
      return true;
    end
  end
  return false;
end

function filelfs:GetFileSystemTree(path)
  local filetree = {};
  for entry in lfs.dir(path) do
    if entry ~= '.' and entry ~= '..' then
      local childpath = path .. '/' .. entry
      childpath = string.gsub(childpath,"//","/");
      local attr = lfs.attributes(childpath);
      if attr ~= nil then
        if attr.mode == "directory" then
          table.insert(filetree,self:GetFileSystemTree(childpath));
        else
          local type = filelfs:getExtension(childpath);
          if type ~= nil and (string.lower(type) == "fbx" or string.lower(type) == "obj") then
            table.insert(filetree,self:GetFbxResource(childpath));
          else
            table.insert(filetree,childpath);
          end
        end
      else
        LOGE("invalid path!");
      end
    end
  end
  self:SortTable(filetree);
  table.insert(filetree,1,path);
  return filetree;
end

function filelfs:DeleteRepeatValue(targettable)
  local hash = {};
  local result = {};
  for _,v in pairs(targettable) do
    if not hash[v] then
      hash[v] = true;
      table.insert(result, v);
    end
  end
  return result;
end

-- 删除path指定的空目录或者文件
function filelfs:RmFile(path)

  local mode = lfs.attributes(path, "mode")
  if mode == "directory" then
    local succ, des = lfs.rmdir(path) -- empty directory
    if not succ then
      LOGE("[rmdir] remove dir "..path.." failed, reason: "..tostring(des))
    end
    return succ;
  else
    -- windows os.remove empty directory will fail, use lfs.rmdir instead
    local succ, des = os.remove(path)
    if not succ then
      LOGE("[rmdir] remove file "..path.." failed, reason: "..tostring(des))
    end
    return succ;
  end

end

-- 删除path整个目录
function filelfs:RmDir(path)
  local succ = true ;
  if self:exists(path) then
    local iter, dir_obj = lfs.dir(path)
    while true do
      local dir = iter(dir_obj)
      if dir == nil then
        break
      end
      if dir ~= "." and dir ~= ".." then
        local curDir = path .. "/" .. dir
        local mode = lfs.attributes(curDir, "mode")
        if mode == "directory" then
          succ = self:RmDir(curDir.."/")
          if not succ then
            LOGE("[rmdir] remove dir "..curDir.." fail")
            break;
          end
        elseif mode == "file" then
          succ  = self:RmFile(curDir)
          if not succ then
            LOGE("[rmdir] remove file 1 "..curDir.." fail");
          end
        else
          LOGE("invalid mode: " .. tostring(mode))
        end
      end
    end
    succ  = self:RmFile(path)
    if not succ then
      LOGE("[rmdir] remove file 2 "..path.." fail");
    end
  end
  return succ
end

-- 创建目录
function filelfs:MkDir(path)
  local ok, err = lfs.mkdir(path);
  if not ok then
    LOGE("[copyDir] mkdir fail, reason "..tostring(err));
  end
  return ok
end


-- 拷贝一个文件或者目录到另外一个目录下
-- 过滤函数 filter 传入当前源文件全路径 返回true 过滤
-- e.g srcPath = "facecute/assets/resource/" targetDir="venusroot/assets"
-- it will mkdir 'resource' in targetDir , and then copy recursively
function filelfs:CopyDir(srcPath, targetDir, filter)
  local succ = false;
  local attr = lfs.attributes(srcPath);
  local filename = filelfs:getFileNameWithPosfix(srcPath);
  local targetpath = targetDir .. "/" .. filename;
  if attr.mode ~= "directory" then

    if filter~=nil and filter(srcPath) then
      return true
    end

    local sourcefile = io.open(srcPath,"rb");
    local destinationfile = io.open(targetpath, "wb");

    local doCopy = function()

      if not sourcefile  then
        LOGE("[CopyDir] open src: "..srcPath.." fail");
        return false;
      end

      if not destinationfile  then
        LOGE("[CopyDir] open src: "..targetpath.." fail");
        return false;
      end

      local len = sourcefile:seek("end");
      if len ~= 0 then
        sourcefile:seek("set",0);
        local size = 2^13      -- good buffer size (8K)
        while true do
          local block = sourcefile:read(size)
          if not block then
            break -- end of file
          end
          local ok, err = destinationfile:write(block)
          if not ok then
            LOGE("[CopyDir] write fail from "..tostring(srcPath).." to "..tostring(targetpath)..", reason:"..tostring(err));
            return false;
          end
        end
      end -- len~=0

      return true;

    end -- doCopy

    succ = doCopy();

    if sourcefile then
      sourcefile:close();
    end
    if destinationfile then
      destinationfile:close();
    end

  else
    succ = filelfs:MkDir(targetpath);
    if not succ then
      LOGE("[CopyDir] fail to mkdir "..tostring(targetpath));
      return false ;
    end
    for entry in lfs.dir(srcPath) do
      if entry ~= "." and entry ~= ".." then
        local temp = srcPath .. "/" .. entry;
        succ = filelfs:CopyDir(temp,targetpath,filter);
        if not succ then
          LOGE("[CopyDir] copy dir "..tostring(temp).." to "..tostring(targetpath).." fail");
          return false ;
        end
      end
    end
  end
  return succ;
end


-- 生成 git version 信息
-- facecuteGit : 項目脚本目录
-- kratosGit : 编辑器脚本目录
function filelfs:GenerateGitInfo(facecuteGit, kratosGit, targetFile)

  if filelfs:exists(targetFile) then
    local succ = filelfs:RmDir(targetFile);
    if not succ then
      LOGE("[generateGitInfo] remove targetFile fail "..tostring(targetFile))
      return false ;
    end
  end

  local command = 'git -C '..tostring(facecuteGit)..' rev-parse HEAD '
  local t = io.popen(command)
  if not t then
    LOGE("[generateGitInfo] execute fail "..command);
    return false ;
  end

  local facecuteGitVersion = t:read("*l")
  t.close();

  command = 'git -C '..tostring(kratosGit)..' rev-parse HEAD '
  t = io.popen(command)
  if not t then
    LOGE("[generateGitInfo] execute fail "..command);
    return false ;
  end

  local kratosGitVersion = t:read("*l")
  t.close();


  if facecuteGitVersion == nil then
    facecuteGitVersion = "missing"
  end

  if kratosGitVersion == nil then
    kratosGitVersion = "missing"
  end

  local gitFile,err = io.open(targetFile,"wb");
  if not gitFile then
    LOGE("[generateGitInfo] open targetFile fail "..tostring(targetFile)..","..err)
    return false ;
  end
  local ok, err = gitFile:write("facecute: "..facecuteGitVersion.."\n")
  ok = ok and gitFile:write("kratos: ".. kratosGitVersion.."\n")
  if not ok then
    LOGE("[generateGitInfo] write targetFile fail "..tostring(targetFile)..","..err)
    gitFile:close();
    return false ;
  end
  gitFile:close();

  return true ;
end

-- 把buffer的数据 写到文件(覆盖 二进制)
function filelfs:WriteBufferToFile(buffer,targetFile)
  local fileHandle,err = io.open(targetFile,"wb");
  if not fileHandle then
    LOGE("[WriteBufferToFile] open targetFile fail "..tostring(targetFile)..","..err)
    return false ;
  end
  local ok, err = fileHandle:write(buffer)
  if not ok then
    LOGE("[WriteBufferToFile] write targetFile fail "..tostring(targetFile)..","..err)
    fileHandle:close();
    return false ;
  end
  fileHandle:close();
  return true ;
end

function filelfs:GetFbxResource(path)
  local fbxresource = {};
  local metadata = self:GetMetaData(path);
  if metadata ~= nil then
    local fbxresourcepath = metadata["outputpath"];
    if fbxresourcepath ~= nil then
      fbxresourcepath =  Core.IFileSystem:Instance():PathAssembly(fbxresourcepath);
      if self:exists(fbxresourcepath) then
        fbxresource = self:GetFileSystemTree(fbxresourcepath);
      end
    end
  end
  fbxresource[1] = path;
  return fbxresource;
end

function filelfs:GetFbxbundle(fbxpath)
  local assetsPath = Core.IFileSystem:Instance():PathAssembly(SystemDirectories["assets"]);
  local libraryPath = Core.IFileSystem:Instance():PathAssembly(SystemDirectories["library"]);
  local fbxresourcepath = fbxpath;
  local _, sEnd = string.find(fbxpath,assetsPath,1,true);
  if sEnd ~= nil then
    fbxresourcepath = libraryPath .. string.sub(fbxpath,sEnd + 1);
  end
  local fbxbundlepath = filelfs:getFileName(fbxresourcepath) .. ".prefab";
  return fbxbundlepath;
end

function filelfs:GetObjbundle(objPath)
  local assetsPath = Core.IFileSystem:Instance():PathAssembly(SystemDirectories["assets"]);
  local libraryPath = Core.IFileSystem:Instance():PathAssembly(SystemDirectories["library"]);
  local objResourcePath = objPath;
  local _, sEnd = string.find(objPath,assetsPath,1,true);
  if sEnd ~= nil then
    objResourcePath = libraryPath .. string.sub(objPath,sEnd + 1);
  end
  local objBundlePath = objResourcePath .. ".prefab";
  return objBundlePath;
end


function filelfs:GetDirectoryInFileTree(path, projectPath, projectFileTree)
  local abpath = Core.IFileSystem:Instance():PathAssembly(path);
  abpath = string.gsub(abpath,"//","/");
  local assetspath = projectPath or Core.IFileSystem:Instance():PathAssembly(SystemDirectories["assets"]);
  local temptree = projectFileTree or FileTree;
  local _, sEnd = string.find(abpath,assetspath,1,true);
  if sEnd ~= nil then
    local relativepath = string.sub(abpath,sEnd + 1);
    local temp = string.split(relativepath, "/");
    for i = 1, #temp do
      assetspath = assetspath .. "/" .. temp[i];
      assetspath = string.gsub(assetspath,"//","/");
      temptree = self:FindFile(assetspath,temptree);
      if temptree == nil then
        LOGE("file path do not exist");
        return false;
      end
    end
  end
  return temptree;
end

function filelfs:FindFile(path, tree)
  for i = 2, #tree do
    if path == tree[i][1] then
      return tree[i];
    end
  end
end

function filelfs:SortTable(tree)
  if next(tree) == nil then
    return tree;
  end
  table.sort(tree,function (element1,element2)
    if type(element1) == "table" and type(element2) == "table" then
      return element1[1] < element2[1];
    elseif type(element1) ~= "table" and type(element2) == "table" then
      return false;
    elseif type(element1) == "table" and type(element2) ~= "table" then
      return true;
    else
      return element1 < element2;
    end
  end);
end

function filelfs:DirectoryExistFile(directory, path)
  for i = 2, #directory do
    local filepath = nil;
    if type(directory[i]) == "table" then
      filepath = directory[i][1];
    else
      filepath = directory[i];
    end
    if filepath == path then
      return true,i;
    end
  end
  return false,nil;
end

function filelfs:MkDirs(path)
  local base = ""
  if string.find(path, "/") == 1 then
    base = "/"
  end
  local splitPath = filelfs:stringsplit(path, "/")
  for i = 1, #splitPath do
    if i == 1 then
      base = base .. splitPath[i]
    else
      base = base .. "/" .. splitPath[i]
    end
    if not filelfs:exists(base) then
      local ok, err = lfs.mkdir(base)
    end
  end
  return true
end

--检查中文，存在则返回true
function filelfs:CheckChinese(str) 
	local f = '[%z\1-\127\194-\244][\128-\191]*';
	for v in string.gmatch(str,f) do  
		local isChinese = (#v~=1);
		if isChinese then
			return true
		end
	end
	return false;
end

function filelfs:NormalizePath(path)
  local filePath = path;
  if filePath ~= nil then
    filePath = string.gsub(filePath,"\\","/");
    filePath = string.gsub(filePath,"/+","/");
    local temp = string.split(filePath, [[/]]);
    while self:Contains(temp, "..") or self:Contains(temp, ".") do
      local removelist = {};
      for i = 1, #temp do
        if temp[i] == ".." then
          table.insert(removelist,i);
          table.insert(removelist,i-1);
          break;
        elseif temp[i] == "." then
          table.insert(removelist, i);
          break;
        end
      end
      for i = 1,#removelist do
        table.remove(temp,removelist[i]);
      end
    end
    filePath = "";
    for i = 1, #temp do
      if i == #temp then
        filePath = filePath .. temp[i];
      else
        filePath = filePath .. temp[i] .. [[/]];
      end
    end
  end
  return filePath;
end

function filelfs:DeleteFileByType(path, type)
  for entry in lfs.dir(path) do
    if entry ~= '.' and entry ~= '..' then
      local childpath = path .. '/' .. entry
      childpath = string.gsub(childpath,"//","/");
      local attr = lfs.attributes(childpath);
      if attr ~= nil then
        if attr.mode == "directory" then
          self:DeleteFileByType(childpath, type);
        else
          if filelfs:getExtension(childpath) == type then
            os.remove(childpath);
          end
        end
      end
    end
  end
end



function filelfs:GetLastModifyTime(path)
  local attr = lfs.attributes(path);
  if attr ~= nil then
    return attr.modification;
  end
  return nil;
end


return filelfs;
