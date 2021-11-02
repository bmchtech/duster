require 'vector'
require 'sidebean'
require 'duster'

-- `cexport` marks this function to be exported
-- `codename` fix the generated C code name
local function bap_nelua_add(a: int32, b: int32): int32 <cexport, codename 'bap_nelua_add'>
    local vec: vector(int32) = {}

    vec:push(a)
    vec:push(b)

    local t2 = vec:pop()
    local t1 = vec:pop()

    vec:destroy()

    bap_saysomething("yeet from the lua :sunglasses:")

    return internal_add_numbers(t1, t2)
end