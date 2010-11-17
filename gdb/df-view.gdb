#
# print a dune-fem discrete function
#

define pdf
	if $argc == 0
		help pdf
	else
		set $gridSize = $arg0.spc_.gridPart_.grid_.mapper.ne[0]
                set $numDofs = $arg0.dofVec_.size_
	if $argc == 1
		set $i = 0
		while $i < $numDofs
			printf "elem[%u]: ", $i
			p $arg0.dofVec_.vec_[$i]
			set $i++
		end
	end
	if $argc > 0
		printf "Number of dofs = %u\n", $numDofs
		printf "Grid size = %u\n", $gridSize
	end
	end
end

document pdf
        Prints discrete function information
        Syntax pdf <discretefunction>
end 

