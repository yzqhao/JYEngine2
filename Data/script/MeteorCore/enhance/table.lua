

function table.parint(root)
  local cache = {  [root] = "." }
  local function _dump(t,space,name)
    local temp = {}
    for k,v in pairs(t) do
      local key = tostring(k)
      if cache[v] then
        table.insert(temp,"+" .. key .. " {" .. cache[v].."}")
      elseif type(v) == "table" then
        local new_key = name .. "." .. key
        cache[v] = new_key
        table.insert(temp,"+" .. key .. _dump(v,space .. (next(t,k) and "|" or " " ).. string.rep(" ",#key),new_key))
      else
        table.insert(temp,"+" .. key .. " [" .. tostring(v).."]")
      end
    end
    return table.concat(temp,"\n"..space)
  end
  local function magiclines(s)
        if s:sub(-1)~="\n" then s=s.."\n" end
        return s:gmatch("(.-)\n")
  end
  local str = _dump(root, "","");
  for line in magiclines(str) do
    LOGI(line);
  end
end

function table.marge(first_table, second_table)
  for k,v in pairs(second_table) do
    first_table[k] = v
  end
end

function table.append(...)
  local new = {};
  if next({...}) then
    -- get the count of the params
    for i = 1, select('#', ...) do
      -- select the param
      local param = select(i, ...)
      for k, v in pairs(param) do
        table.insert(new, v);
      end
    end
  else
    print("empty var")
  end
  return new;
end

function table.getlength(target)
  local i = 0;
  if target then
    for k, v in pairs(target) do
      i = i + 1;
    end
  end
  return i;
end

function table.getkeybyvalue(target, value)
  for k,v in pairs(target) do
    if v == value then
      return k;
    end
  end
  return nil;
end

local function _deepcopy(orig)
    local orig_type = type(orig)
    local copy
    if orig_type == 'table' then
        copy = {}
        for orig_key, orig_value in next, orig, nil do
            copy[_deepcopy(orig_key)] = _deepcopy(orig_value)
        end
        setmetatable(copy, _deepcopy(getmetatable(orig)))
    else -- number, string, boolean, etc
        copy = orig
    end
    return copy
end
table.deepcopy = _deepcopy;