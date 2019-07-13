local Arrays = {}

function Arrays.array3D(size)
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

function Arrays.array2D(size)
	local arr = {}
	for i=0,size-1 do
		arr[i] = {}
		for j=0,size-1 do
			arr[i][j] = 0
		end
	end

	return arr
end

return Arrays