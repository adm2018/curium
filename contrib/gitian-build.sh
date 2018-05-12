<<<<<<< HEAD
=======
#!/bin/bash
>>>>>>> dev-1.12.1.0
# Copyright (c) 2016 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

# What to do
sign=false
verify=false
build=false
<<<<<<< HEAD
setupenv=false
=======
>>>>>>> dev-1.12.1.0

# Systems to build
linux=true
windows=true
osx=true

# Other Basic variables
SIGNER=
VERSION=
commit=false
<<<<<<< HEAD
url=https://github.com/mrmetech/Curium-Official
=======
url=https://github.com/curiumofficial/curium
>>>>>>> dev-1.12.1.0
proc=2
mem=2000
lxc=true
osslTarUrl=http://downloads.sourceforge.net/project/osslsigncode/osslsigncode/osslsigncode-1.7.1.tar.gz
osslPatchUrl=https://bitcoincore.org/cfields/osslsigncode-Backports-to-1.7.1.patch
scriptName=$(basename -- "$0")
signProg="gpg --detach-sign"
commitFiles=true

# Help Message
<<<<<<< HEAD
read -d '' usage <<- EOF
Usage: $scriptName [-c|u|v|b|s|B|o|h|j|m|] signer version

Run this script from the directory containing the Curium-Official, gitian-builder, gitian.sigs, and Curium-Official-detached-sigs.
=======
read -r -d '' usage <<- EOF
Usage: $scriptName [-c|u|v|b|s|B|o|h|j|m|] signer version

Run this script from the directory containing the curium, gitian-builder, gitian.sigs, and curium-detached-sigs.
>>>>>>> dev-1.12.1.0

Arguments:
signer          GPG signer to sign each build assert file
version		Version number, commit, or branch to build. If building a commit or branch, the -c option must be specified

Options:
-c|--commit	Indicate that the version argument is for a commit or branch
<<<<<<< HEAD
-u|--url	Specify the URL of the repository. Default is https://github.com/Curium-Officialproject/Curium-Official
=======
-u|--url	Specify the URL of the repository. Default is https://github.com/curiumofficial/curium
>>>>>>> dev-1.12.1.0
-v|--verify 	Verify the gitian build
-b|--build	Do a gitian build
-s|--sign	Make signed binaries for Windows and Mac OSX
-B|--buildsign	Build both signed and unsigned binaries
-o|--os		Specify which Operating Systems the build is for. Default is lwx. l for linux, w for windows, x for osx, a for aarch64
-j		Number of processes to use. Default 2
-m		Memory to allocate in MiB. Default 2000
--kvm           Use KVM instead of LXC
--setup         Setup the gitian building environment. Uses KVM. If you want to use lxc, use the --lxc option. Only works on Debian-based systems (Ubuntu, Debian)
--detach-sign   Create the assert file for detached signing. Will not commit anything.
--no-commit     Do not commit anything to git
-h|--help	Print this help message
EOF

# Get options and arguments
while :; do
    case $1 in
        # Verify
        -v|--verify)
	    verify=true
            ;;
        # Build
        -b|--build)
	    build=true
            ;;
        # Sign binaries
        -s|--sign)
	    sign=true
            ;;
        # Build then Sign
        -B|--buildsign)
	    sign=true
	    build=true
            ;;
        # PGP Signer
        -S|--signer)
	    if [ -n "$2" ]
	    then
		SIGNER=$2
		shift
	    else
		echo 'Error: "--signer" requires a non-empty argument.'
		exit 1
	    fi
           ;;
        # Operating Systems
        -o|--os)
	    if [ -n "$2" ]
	    then
		linux=false
		windows=false
		osx=false
		aarch64=false
		if [[ "$2" = *"l"* ]]
		then
		    linux=true
		fi
		if [[ "$2" = *"w"* ]]
		then
		    windows=true
		fi
		if [[ "$2" = *"x"* ]]
		then
		    osx=true
		fi
		if [[ "$2" = *"a"* ]]
		then
		    aarch64=true
		fi
		shift
	    else
<<<<<<< HEAD
		echo 'Error: "--os" requires an argument containing an l (for linux), w (for windows), x (for Mac OSX), or a (for aarch64)\n'
=======
		printf 'Error: "--os" requires an argument containing an l (for linux), w (for windows), x (for Mac OSX), or a (for aarch64)\n'
>>>>>>> dev-1.12.1.0
		exit 1
	    fi
	    ;;
	# Help message
	-h|--help)
	    echo "$usage"
	    exit 0
	    ;;
	# Commit or branch
	-c|--commit)
	    commit=true
	    ;;
	# Number of Processes
	-j)
	    if [ -n "$2" ]
	    then
		proc=$2
		shift
	    else
		echo 'Error: "-j" requires an argument'
		exit 1
	    fi
	    ;;
	# Memory to allocate
	-m)
	    if [ -n "$2" ]
	    then
		mem=$2
		shift
	    else
		echo 'Error: "-m" requires an argument'
		exit 1
	    fi
	    ;;
	# URL
	-u)
	    if [ -n "$2" ]
	    then
		url=$2
		shift
	    else
		echo 'Error: "-u" requires an argument'
		exit 1
	    fi
	    ;;
        # kvm
        --kvm)
            lxc=false
            ;;
        # Detach sign
        --detach-sign)
            signProg="true"
            commitFiles=false
            ;;
        # Commit files
        --no-commit)
            commitFiles=false
            ;;
        # Setup
        --setup)
            setup=true
            ;;
	*)               # Default case: If no more options then break out of the loop.
             break
    esac
    shift
done

# Set up LXC
if [[ $lxc = true ]]
then
    export USE_LXC=1
    export LXC_BRIDGE=lxcbr0
    sudo ifconfig lxcbr0 up 10.0.2.2
fi

# Check for OSX SDK
<<<<<<< HEAD
if [[ ! -e "gitian-builder/inputs/MacOSX10.11.sdk.tar.gz" && $osx == true ]]
=======
if [[ ! -e "gitian-builder/inputs/MacOSX10.9.sdk.tar.gz" && $osx == true ]]
>>>>>>> dev-1.12.1.0
then
    echo "Cannot build for OSX, SDK does not exist. Will build for other OSes"
    osx=false
fi

# Get signer
<<<<<<< HEAD
if [[ -n"$1" ]]
=======
if [[ -n "$1" ]]
>>>>>>> dev-1.12.1.0
then
    SIGNER=$1
    shift
fi

# Get version
if [[ -n "$1" ]]
then
    VERSION=$1
    COMMIT=$VERSION
    shift
fi

# Check that a signer is specified
if [[ $SIGNER == "" ]]
then
    echo "$scriptName: Missing signer."
    echo "Try $scriptName --help for more information"
    exit 1
fi

# Check that a version is specified
if [[ $VERSION == "" ]]
then
    echo "$scriptName: Missing version."
    echo "Try $scriptName --help for more information"
    exit 1
fi

# Add a "v" if no -c
if [[ $commit = false ]]
then
	COMMIT="v${VERSION}"
fi
<<<<<<< HEAD
echo ${COMMIT}
=======
echo "${COMMIT}"
>>>>>>> dev-1.12.1.0

# Setup build environment
if [[ $setup = true ]]
then
    sudo apt-get install ruby apache2 git apt-cacher-ng python-vm-builder qemu-kvm qemu-utils
    git clone https://github.com/phoreproject/gitian.sigs.git
    git clone https://github.com/phoreproject/phore-detached-sigs.git
    git clone https://github.com/devrandom/gitian-builder.git
<<<<<<< HEAD
    pushd ./gitian-builder
=======
    pushd ./gitian-builder || exit
>>>>>>> dev-1.12.1.0
    if [[ -n "$USE_LXC" ]]
    then
        sudo apt-get install lxc
        bin/make-base-vm --suite trusty --arch amd64 --lxc
    else
        bin/make-base-vm --suite trusty --arch amd64
    fi
<<<<<<< HEAD
    popd
fi

# Set up build
pushd ./Curium-Official
git fetch
git checkout ${COMMIT}
popd
=======
    popd || exit
fi

# Set up build
pushd ./curium || exit
git fetch
git checkout "${COMMIT}"
popd || exit
>>>>>>> dev-1.12.1.0

# Build
if [[ $build = true ]]
then
	# Make output folder
<<<<<<< HEAD
	mkdir -p ./Curium-Official-binaries/${VERSION}
=======
	mkdir -p "./curium-binaries/${VERSION}"
>>>>>>> dev-1.12.1.0

	# Build Dependencies
	echo ""
	echo "Building Dependencies"
	echo ""
<<<<<<< HEAD
	pushd ./gitian-builder
	mkdir -p inputs
	wget -N -P inputs $osslPatchUrl
	wget -N -P inputs $osslTarUrl
	make -C ../Curium-Official/depends download SOURCES_PATH=`pwd`/cache/common
=======
	pushd ./gitian-builder || exit
	mkdir -p inputs
	wget -N -P inputs $osslPatchUrl
	wget -N -P inputs $osslTarUrl
	make -C ../curium/depends download SOURCES_PATH="$(pwd)/cache/common"
>>>>>>> dev-1.12.1.0

	# Linux
	if [[ $linux = true ]]
	then
            echo ""
	    echo "Compiling ${VERSION} Linux"
	    echo ""
<<<<<<< HEAD
	    ./bin/gbuild -j ${proc} -m ${mem} --commit Curium-Official=${COMMIT} --url Curium-Official=${url} ../Curium-Official/contrib/gitian-descriptors/gitian-linux.yml
	    ./bin/gsign -p $signProg --signer $SIGNER --release ${VERSION}-linux --destination ../gitian.sigs/ ../Curium-Official/contrib/gitian-descriptors/gitian-linux.yml
	    mv build/out/Curium-Official-*.tar.gz build/out/src/Curium-Official-*.tar.gz ../Curium-Official-binaries/${VERSION}
=======
	    ./bin/gbuild -j ${proc} -m ${mem} --commit curium=${COMMIT} --url curium=${url} ../curium/contrib/gitian-descriptors/gitian-linux.yml
	    ./bin/gsign --signer $SIGNER --release ${VERSION}-linux --destination ../gitian.sigs/ ../curium/contrib/gitian-descriptors/gitian-linux.yml
	    mv build/out/curium-*.tar.gz build/out/src/curium-*.tar.gz ../curium-binaries/${VERSION}
>>>>>>> dev-1.12.1.0
	fi
	# Windows
	if [[ $windows = true ]]
	then
	    echo ""
	    echo "Compiling ${VERSION} Windows"
	    echo ""
<<<<<<< HEAD
	    ./bin/gbuild -j ${proc} -m ${mem} --commit Curium-Official=${COMMIT} --url Curium-Official=${url} ../Curium-Official/contrib/gitian-descriptors/gitian-win.yml
	    ./bin/gsign -p $signProg --signer $SIGNER --release ${VERSION}-win-unsigned --destination ../gitian.sigs/ ../Curium-Official/contrib/gitian-descriptors/gitian-win.yml
	    mv build/out/Curium-Official-*-win-unsigned.tar.gz inputs/Curium-Official-win-unsigned.tar.gz
	    mv build/out/Curium-Official-*.zip build/out/Curium-Official-*.exe ../Curium-Official-binaries/${VERSION}
=======
	    ./bin/gbuild -j ${proc} -m ${mem} --commit curium=${COMMIT} --url curium=${url} ../curium/contrib/gitian-descriptors/gitian-win.yml
	    ./bin/gsign --signer $SIGNER --release ${VERSION}-win-unsigned --destination ../gitian.sigs/ ../curium/contrib/gitian-descriptors/gitian-win.yml
	    mv build/out/curium-*-win-unsigned.tar.gz inputs/curium-win-unsigned.tar.gz
	    mv build/out/curium-*.zip build/out/curium-*.exe ../curium-binaries/${VERSION}
>>>>>>> dev-1.12.1.0
	fi
	# Mac OSX
	if [[ $osx = true ]]
	then
	    echo ""
	    echo "Compiling ${VERSION} Mac OSX"
	    echo ""
<<<<<<< HEAD
	    ./bin/gbuild -j ${proc} -m ${mem} --commit Curium-Official=${COMMIT} --url Curium-Official=${url} ../Curium-Official/contrib/gitian-descriptors/gitian-osx.yml
	    ./bin/gsign -p $signProg --signer $SIGNER --release ${VERSION}-osx-unsigned --destination ../gitian.sigs/ ../Curium-Official/contrib/gitian-descriptors/gitian-osx.yml
	    mv build/out/Curium-Official-*-osx-unsigned.tar.gz inputs/Curium-Official-osx-unsigned.tar.gz
	    mv build/out/Curium-Official-*.tar.gz build/out/Curium-Official-*.dmg ../Curium-Official-binaries/${VERSION}
=======
	    ./bin/gbuild -j ${proc} -m ${mem} --commit curium=${COMMIT} --url curium=${url} ../curium/contrib/gitian-descriptors/gitian-osx.yml
	    ./bin/gsign --signer $SIGNER --release ${VERSION}-osx-unsigned --destination ../gitian.sigs/ ../curium/contrib/gitian-descriptors/gitian-osx.yml
	    mv build/out/curium-*-osx-unsigned.tar.gz inputs/curium-osx-unsigned.tar.gz
	    mv build/out/curium-*.tar.gz build/out/curium-*.dmg ../curium-binaries/${VERSION}
>>>>>>> dev-1.12.1.0
	fi
	# AArch64
	if [[ $aarch64 = true ]]
	then
	    echo ""
	    echo "Compiling ${VERSION} AArch64"
	    echo ""
<<<<<<< HEAD
	    ./bin/gbuild -j ${proc} -m ${mem} --commit Curium-Official=${COMMIT} --url Curium-Official=${url} ../Curium-Official/contrib/gitian-descriptors/gitian-aarch64.yml
	    ./bin/gsign -p $signProg --signer $SIGNER --release ${VERSION}-aarch64 --destination ../gitian.sigs/ ../Curium-Official/contrib/gitian-descriptors/gitian-aarch64.yml
	    mv build/out/Curium-Official-*.tar.gz build/out/src/Curium-Official-*.tar.gz ../Curium-Official-binaries/${VERSION}
	popd
=======
	    ./bin/gbuild -j ${proc} -m ${mem} --commit curium=${COMMIT} --url curium=${url} ../curium/contrib/gitian-descriptors/gitian-aarch64.yml
	    ./bin/gsign --signer $SIGNER --release ${VERSION}-aarch64 --destination ../gitian.sigs/ ../curium/contrib/gitian-descriptors/gitian-aarch64.yml
	    mv build/out/curium-*.tar.gz build/out/src/curium-*.tar.gz ../curium-binaries/${VERSION}
	fi
	popd || exit
>>>>>>> dev-1.12.1.0

        if [[ $commitFiles = true ]]
        then
	    # Commit to gitian.sigs repo
            echo ""
            echo "Committing ${VERSION} Unsigned Sigs"
            echo ""
<<<<<<< HEAD
            pushd gitian.sigs
=======
            pushd gitian.sigs || exit
>>>>>>> dev-1.12.1.0
            git add ${VERSION}-linux/${SIGNER}
            git add ${VERSION}-aarch64/${SIGNER}
            git add ${VERSION}-win-unsigned/${SIGNER}
            git add ${VERSION}-osx-unsigned/${SIGNER}
            git commit -a -m "Add ${VERSION} unsigned sigs for ${SIGNER}"
<<<<<<< HEAD
            popd
=======
            popd || exit
>>>>>>> dev-1.12.1.0
        fi
fi

# Verify the build
if [[ $verify = true ]]
then
	# Linux
<<<<<<< HEAD
	pushd ./gitian-builder
	echo ""
	echo "Verifying v${VERSION} Linux"
	echo ""
	./bin/gverify -v -d ../gitian.sigs/ -r ${VERSION}-linux ../Curium-Official/contrib/gitian-descriptors/gitian-linux.yml
=======
	pushd ./gitian-builder || exit
	echo ""
	echo "Verifying v${VERSION} Linux"
	echo ""
	./bin/gverify -v -d ../gitian.sigs/ -r ${VERSION}-linux ../curium/contrib/gitian-descriptors/gitian-linux.yml
>>>>>>> dev-1.12.1.0
	# Windows
	echo ""
	echo "Verifying v${VERSION} Windows"
	echo ""
<<<<<<< HEAD
	./bin/gverify -v -d ../gitian.sigs/ -r ${VERSION}-win-unsigned ../Curium-Official/contrib/gitian-descriptors/gitian-win.yml
=======
	./bin/gverify -v -d ../gitian.sigs/ -r ${VERSION}-win-unsigned ../curium/contrib/gitian-descriptors/gitian-win.yml
>>>>>>> dev-1.12.1.0
	# Mac OSX
	echo ""
	echo "Verifying v${VERSION} Mac OSX"
	echo ""
<<<<<<< HEAD
	./bin/gverify -v -d ../gitian.sigs/ -r ${VERSION}-osx-unsigned ../Curium-Official/contrib/gitian-descriptors/gitian-osx.yml
=======
	./bin/gverify -v -d ../gitian.sigs/ -r ${VERSION}-osx-unsigned ../curium/contrib/gitian-descriptors/gitian-osx.yml
>>>>>>> dev-1.12.1.0
	# AArch64
	echo ""
	echo "Verifying v${VERSION} AArch64"
	echo ""
<<<<<<< HEAD
	./bin/gverify -v -d ../gitian.sigs/ -r ${VERSION}-aarch64 ../Curium-Official/contrib/gitian-descriptors/gitian-aarch64.yml
=======
	./bin/gverify -v -d ../gitian.sigs/ -r ${VERSION}-aarch64 ../curium/contrib/gitian-descriptors/gitian-aarch64.yml
>>>>>>> dev-1.12.1.0
	# Signed Windows
	echo ""
	echo "Verifying v${VERSION} Signed Windows"
	echo ""
<<<<<<< HEAD
	./bin/gverify -v -d ../gitian.sigs/ -r ${VERSION}-osx-signed ../Curium-Official/contrib/gitian-descriptors/gitian-osx-signer.yml
=======
	./bin/gverify -v -d ../gitian.sigs/ -r ${VERSION}-osx-signed ../curium/contrib/gitian-descriptors/gitian-osx-signer.yml
>>>>>>> dev-1.12.1.0
	# Signed Mac OSX
	echo ""
	echo "Verifying v${VERSION} Signed Mac OSX"
	echo ""
<<<<<<< HEAD
	./bin/gverify -v -d ../gitian.sigs/ -r ${VERSION}-osx-signed ../Curium-Official/contrib/gitian-descriptors/gitian-osx-signer.yml
	popd
=======
	./bin/gverify -v -d ../gitian.sigs/ -r ${VERSION}-osx-signed ../curium/contrib/gitian-descriptors/gitian-osx-signer.yml
	popd || exit
>>>>>>> dev-1.12.1.0
fi

# Sign binaries
if [[ $sign = true ]]
then

<<<<<<< HEAD
        pushd ./gitian-builder
=======
        pushd ./gitian-builder || exit
>>>>>>> dev-1.12.1.0
	# Sign Windows
	if [[ $windows = true ]]
	then
	    echo ""
	    echo "Signing ${VERSION} Windows"
	    echo ""
<<<<<<< HEAD
	    ./bin/gbuild -i --commit signature=${COMMIT} ../Curium-Official/contrib/gitian-descriptors/gitian-win-signer.yml
	    ./bin/gsign -p $signProg --signer $SIGNER --release ${VERSION}-win-signed --destination ../gitian.sigs/ ../Curium-Official/contrib/gitian-descriptors/gitian-win-signer.yml
	    mv build/out/Curium-Official-*win64-setup.exe ../Curium-Official-binaries/${VERSION}
	    mv build/out/Curium-Official-*win32-setup.exe ../Curium-Official-binaries/${VERSION}
=======
	    ./bin/gbuild -i --commit signature=${COMMIT} ../curium/contrib/gitian-descriptors/gitian-win-signer.yml
	    ./bin/gsign --signer $SIGNER --release ${VERSION}-win-signed --destination ../gitian.sigs/ ../curium/contrib/gitian-descriptors/gitian-win-signer.yml
	    mv build/out/curium-*win64-setup.exe ../curium-binaries/${VERSION}
	    mv build/out/curium-*win32-setup.exe ../curium-binaries/${VERSION}
>>>>>>> dev-1.12.1.0
	fi
	# Sign Mac OSX
	if [[ $osx = true ]]
	then
	    echo ""
	    echo "Signing ${VERSION} Mac OSX"
	    echo ""
<<<<<<< HEAD
	    ./bin/gbuild -i --commit signature=${COMMIT} ../Curium-Official/contrib/gitian-descriptors/gitian-osx-signer.yml
	    ./bin/gsign -p $signProg --signer $SIGNER --release ${VERSION}-osx-signed --destination ../gitian.sigs/ ../Curium-Official/contrib/gitian-descriptors/gitian-osx-signer.yml
	    mv build/out/Curium-Official-osx-signed.dmg ../Curium-Official-binaries/${VERSION}/Curium-Official-${VERSION}-osx.dmg
	fi
	popd
=======
	    ./bin/gbuild -i --commit signature=${COMMIT} ../curium/contrib/gitian-descriptors/gitian-osx-signer.yml
	    ./bin/gsign --signer $SIGNER --release ${VERSION}-osx-signed --destination ../gitian.sigs/ ../curium/contrib/gitian-descriptors/gitian-osx-signer.yml
	    mv build/out/curium-osx-signed.dmg ../curium-binaries/${VERSION}/curium-${VERSION}-osx.dmg
	fi
	popd || exit
>>>>>>> dev-1.12.1.0

        if [[ $commitFiles = true ]]
        then
            # Commit Sigs
<<<<<<< HEAD
            pushd gitian.sigs
=======
            pushd gitian.sigs || exit
>>>>>>> dev-1.12.1.0
            echo ""
            echo "Committing ${VERSION} Signed Sigs"
            echo ""
            git add ${VERSION}-win-signed/${SIGNER}
            git add ${VERSION}-osx-signed/${SIGNER}
            git commit -a -m "Add ${VERSION} signed binary sigs for ${SIGNER}"
<<<<<<< HEAD
            popd
=======
            popd || exit
>>>>>>> dev-1.12.1.0
        fi
fi
