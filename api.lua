
EnumDirection = {
	POSITIVE_X = 0,
    NEGATIVE_X = 1,
    POSITIVE_Y = 2,
    NEGATIVE_Y = 3,
    POSITIVE_Z = 4,
    NEGATIVE_Z = 5
}


function rgb(r, g, b)
	return {r / 255, g / 255, b / 255}
end

--vecmath
vec3Metatable = {}

function vec3Metatable.__add(f1, f2)
	f1.x = f1.x + f2.x;
	f1.y = f1.y + f2.y;
	f1.z = f1.z + f2.z;
	return f1;
end

function vec3Metatable.__sub(f1, f2)
	f1.x = f1.x - f2.x;
	f1.y = f1.y - f2.y;
	f1.z = f1.z - f2.z;
	return f1;
end

function vec3Metatable.__mul(f1, f2)
	f1.x = f1.x * f2.x;
	f1.y = f1.y * f2.y;
	f1.z = f1.z * f2.z;
	return f1;
end

function vec3Metatable.__div(f1, f2)
	f1.x = f1.x / f2.x;
	f1.y = f1.y / f2.y;
	f1.z = f1.z / f2.z;
	return f1;
end

function vec3(X, Y, Z)
	local obj = {x = X, y = Y, z = Z }
	return setmetatable(obj, vec3Metatable)
end

function lengthsquared(vec)
	return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
end

function length(vec)
	return math.sqrt(lengthsquared(vec))
end

function normalize(vec)
	local l = length(vec)
	return vec / vec3(l, l, l)
end

function cross(lhs, rhs)
	local x = lhs.y * rhs.z - lhs.z * rhs.y;
	local y = lhs.z * rhs.x - lhs.x * rhs.z;
	local z = lhs.x * rhs.y - lhs.y * rhs.x;
	return vec3(x, y, z);
end

function dot(lhs, rhs)
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
end

function array3D(size)
	local arr = {}
	for i=1,size do
		arr[i] = {}
		for j=1,size do
			arr[i][j] = {}
			for k=1,size do
				arr[i][j][k] = 0
			end
		end
	end

	return arr
end

function array2D(size)
	local arr = {}
	for i=0,size-1 do
		arr[i] = {}
		for j=0,size-1 do
			arr[i][j] = 0
		end
	end

	return arr
end