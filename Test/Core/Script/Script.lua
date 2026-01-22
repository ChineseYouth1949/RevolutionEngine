print("[Lua] ptr : ", ptr)
print("[Lua] ptr.value : ", ptr.value)
print("[Lua] ptr:getValue() : ", ptr:getValue())

for i = 1, 5 do
    ptr:setValue(i * 10)
    print("[Lua] ptr:getValue() : ", ptr:getValue())
end
    
ptr.value = 1

local final_value = ptr:getValue()
print("[Lua] final_value:", final_value)
    
return final_value