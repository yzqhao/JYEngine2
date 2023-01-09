



string.split = function(s, p)
    local rt= {}
    string.gsub(s, '[^'..p..']+', function(w) table.insert(rt, w) end )
    return rt
end

string.caseinsensitive = function (pattern)

  -- find an optional '%' (group 1) followed by any character (group 2)
  local p = pattern:gsub("(%%?)(.)", function(percent, letter)

    if percent ~= "" or not letter:match("%a") then
      -- if the '%' matched, or `letter` is not a letter, return "as is"
      return percent .. letter
    else
      -- else, return a case-insensitive character class of the matched letter
      return string.format("[%s%s]", letter:lower(), letter:upper())
    end

  end)

  return p
end


string.startsWith = function(origin, prefix)
  return string.sub(origin,1, string.len(prefix))==prefix
end


string.endsWith = function(origin, suffix)
  return suffix=='' or string.sub(origin,-string.len(suffix))==suffix
end
