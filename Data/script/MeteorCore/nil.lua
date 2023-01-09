local isNil = function(x)
  if type(x)=="userdata" and  getmetatable(x)==nil then
      return true
  end
  return x==nil;
end
return isNil;