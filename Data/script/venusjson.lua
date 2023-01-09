local Core = require "Core"
local cjson = require "cjson"


local venusjson = {}
local cachecount = 20;
local cachearray = {}
local keyqueue = {}




--加载指定位置的json文件返回table
function venusjson.LaodJsonFile(path)
  local res = cachearray[path];
  local lastmodif = Core.IFileSystem:Instance():GetTimestamp(path);
  if res then    
    if res.timestamp ~= lastmodif then
      res = nil;
    end
  end  
  if not res then
    res = {};
    if not Core.IFileSystem:Instance():isFileExist(path) then
      LOGE("json file:" .. path .. " not exist");
    else    
      local str = Core.IFileSystem:Instance():ReadFile(path);
      res.json = cjson.decode(str);
      res.timestamp = lastmodif;
      cachearray[path] = res;
      local size = #keyqueue;
      keyqueue[size + 1] = path;
      if size == cachecount then
        local rp = table.remove(keyqueue, 1);
        cachearray[rp] = nil;
      end      
    end
  end
  return table.deepcopy(res.json);
end

venusjson.LoadJsonFile = venusjson.LaodJsonFile

function venusjson.LoadJsonString(text)
    if text == nil then
        return nil
    end
    local json = cjson.decode(text)
    if json == nil then
        return nil
    end
    return table.deepcopy(json)
end

function venusjson.StoreJsonString(json)
    if json == nil then
        return nil
    end
    return cjson.encode(json)
end

function venusjson.StoreSparseJsonString(jtable)
  cjson.encode_sparse_array(true)
  if jtable == nil then
    return nil
  end
  return cjson.encode(jtable)
end

function venusjson.SaveJsonObject(jtable, path)
  local sf, df, err, ok
  local function bail(...)
		if df then df:close() end
		return ...
	end
  local jstr = cjson.encode(jtable);
  df, err = io.open(path, "wb")
	if not df then return bail(nil, err) end
  ok, err = df:write(jstr)
  if not ok then return bail(nil, err) end
  return bail(true);
end

return venusjson;