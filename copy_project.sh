#!/bin/bash

solutionDir=$(cd "$(dirname $0)"; pwd)
#echo ${solutionDir}

projectsDir=${solutionDir}/Projects
#echo ${projectsDir}

projects=($(ls ${solutionDir} | grep -E "^[0-9]{2}_*"))
#echo ${projects[@]}

if [[ ${#projects[@]} = 0 ]]; then
	echo -e "\033[31mNo projects\033[0m"
	exit 1
fi

oldname=${projects[0]}
vcxprojFile=${solutionDir}/Projects/${oldname}.vcxproj
userFile=${solutionDir}/Projects/${oldname}.vcxproj.user
filtersFile=${solutionDir}/Projects/${oldname}.vcxproj.filters

cd ${projectsDir}

for (( i = 1; i < ${#projects[@]}; ++i )); do
	project=${projects[${i}]}
	echo "vcxproj ${project}"

	if [[ ! -f ${project}.vcxproj.user ]]; then
		if [[ -f ${userFile} ]]; then
			cp -a ${userFile} ${project}.vcxproj.user
			echo -e "\033[33m  copy vcxproj.user \033[0m"

			sed -i "s/${oldname}/${project}/g" ${project}.vcxproj.user
			echo -e "\033[32m  change vcxproj.user ok \033[0m"
		fi
	fi

	if [[ ! -f ${project}.vcxproj.filters ]]; then
		if [[ -f ${filtersFile} ]]; then
			cp -a ${filtersFile} ${project}.vcxproj.filters
			echo -e "\033[33m  copy vcxproj.filters \033[0m"

			sed -i "s/${oldname}/${project}/g" ${project}.vcxproj.filters
			echo -e "\033[32m  change vcxproj.filters ok \033[0m"
		fi
	fi

	if [[ ! -f ${project}.vcxproj ]]; then
		if [[ -f ${vcxprojFile} ]]; then
			cp -a ${vcxprojFile} ${project}.vcxproj
			echo -e "\033[33m  copy vcxproj \033[0m"

			sed -i "s/${oldname}/${project}/g" ${project}.vcxproj
			echo -e "\033[32m  change vcxproj ok \033[0m"
		fi
	fi
done
