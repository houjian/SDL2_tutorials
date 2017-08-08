#!/bin/bash

solutionDir=$(cd "$(dirname $0)"; pwd)
#echo ${solutionDir}

projectsDir=${solutionDir}/Projects
#echo ${projectsDir}

cd ${projectsDir}

solutionName=../${solutionDir##*/}.sln
#echo ${solutionName}

printf '\xEF\xBB\xBF' > ${solutionName}

cat >> ${solutionName} << EOF

Microsoft Visual Studio Solution File, Format Version 12.00
# Visual Studio 15
VisualStudioVersion = 15.0.26430.6
MinimumVisualStudioVersion = 10.0.40219.1
EOF

grep "ProjectGuid" *.vcxproj | awk -F'[.{}]' '{print "Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"" \
	$1 "\", \"Projects\\" $1 ".vcxproj\", \"{" $3 "}\"\r\nEndProject"}' >> ${solutionName}

cat >> ${solutionName} << EOF
Global
	GlobalSection(SolutionConfigurationPlatforms) = preSolution
		Debug|x64 = Debug|x64
		Debug|x86 = Debug|x86
		Release|x64 = Release|x64
		Release|x86 = Release|x86
	EndGlobalSection
	GlobalSection(ProjectConfigurationPlatforms) = postSolution
EOF

grep "ProjectGuid" *.vcxproj | awk -F'[.{}]' '{ \
	print "\t\t{" $3 "}.Debug|x64.ActiveCfg = Debug|x64"; \
	print "\t\t{" $3 "}.Debug|x64.Build.0 = Debug|x64"; \
	print "\t\t{" $3 "}.Debug|x86.ActiveCfg = Debug|Win32"; \
	print "\t\t{" $3 "}.Debug|x86.Build.0 = Debug|Win32"; \
	print "\t\t{" $3 "}.Release|x64.ActiveCfg = Release|x64"; \
	print "\t\t{" $3 "}.Release|x64.Build.0 = Release|x64"; \
	print "\t\t{" $3 "}.Release|x86.ActiveCfg = Release|Win32"; \
	print "\t\t{" $3 "}.Release|x86.Build.0 = Release|Win32";}' >> ${solutionName}

cat >> ${solutionName} << EOF
	EndGlobalSection
	GlobalSection(SolutionProperties) = preSolution
		HideSolutionNode = FALSE
	EndGlobalSection
EndGlobal
EOF
