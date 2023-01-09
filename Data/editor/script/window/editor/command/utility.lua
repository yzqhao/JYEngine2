local Utility = {}


function Utility:CreateInovation(target,method)
  return function(...)
    return method(target,...);
  end
end



return Utility;