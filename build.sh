proj_root="`pwd`"
version_dir="$proj_root/../patches/version/"
on_build_success()
{
        git rev-list HEAD --abbrev-commit  --max-count=1 > tlink_version
        cp tlink_version $version_dir
        cksum tlink
}


set -e
trap "echo '--------------------------Build tlink Exit Error-----------------------------'; vim log" ERR
echo "--------------------------Build tlink Start-----------------------------"

#build tlink
echo > log
tail -f log &
killall tail ; tail -f log &
qmake
make -j32 > log 2>&1
on_build_success

echo "--------------------------Build tlink Exit OK-----------------------------"

exit 0
