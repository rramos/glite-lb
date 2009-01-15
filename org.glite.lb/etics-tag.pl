#!/usr/bin/perl

use Getopt::Std;
use Switch;

$TMPDIR=$ENV{'TMPDIR'};

getopts('i:c:gh');

$module = shift;

$usage = qq{
usage: $0 [-i maj|min|rev|age|<sigle_word_age>] [-g] [-c <current configuration> ] module.name

	-i	What to increment ('maj'or version, 'min'or version, 'rev'ision, 'age')
		Should you fail to specify the -i option the script will open up a cvs diff
		output and ask you to specify what to increment interactively.
	-g	Generate old configuration for comparison
	-c	Use this configuration (\d+\.\d+\.\d+-\S+) rather than parsing version.properties
	-h	Display this help

};

	# **********************************
	# Interpret cmdline options
	# **********************************

	if (defined $opt_h) {die $usage};
	die $usage unless $module;

	switch ($opt_i) {
		case "maj" {$increment="j"}
		case "min" {$increment="i"}
		case "rev" {$increment="r"}
		case "age" {$increment="a"}
		else {$increment=$opt_i};
	}
	

	if (defined $opt_c) {
	
		# **********************************
		# Parse the tag supplied by the user 
		# **********************************

		if ($opt_c=~/(\d+)\.(\d+)\.(\d+)-(\S+?)/) {
				$current_major=$1;
				$current_minor=$2;
				$current_revision=$3;
				$current_age=$4;
		}
		else {die ("tag not stated properly")};

	}
	else {

		# **********************************
		# Determine the most recent tag and its components from version.properties 
		# **********************************

		open VP, "$module/project/version.properties" or die "$module/project/version.properties: $?\n";

		while ($_ = <VP>) {
			chomp;

			if(/module\.version\s*=\s*(\d*)\.(\d*)\.(\d*)/) {
				$current_major=$1;
				$current_minor=$2;
				$current_revision=$3;
			}
			if(/module\.age\s*=\s*(\S+)/) {
				$current_age=$1;
			}
		}
		close (VP);

		$current_prefix=$module;
		$current_prefix=~s/^org\.//;
		$current_prefix=~s/\./-/g;
		$current_prefix="$current_prefix" . "_R_";
		$current_tag="$current_prefix" . "$current_major" . "_$current_minor" . "_$current_revision" . "_$current_age";
	}

	# According to the documentation, symbolic names in the 'cvs log' output are sorted by age so this should be OK
	#$current_tag=`cvs log -h $module/Makefile | grep \"_R_\" | head -n 1`;
	#$current_tag=~s/^\s//;
	#$current_tag=~s/:.*?$//;
	#chomp($current_tag);

	#$current_tag=~/(.*_R_)(\d*?)_(\d*?)_(\d*?)_(.*)/;
	#$current_prefix=$1;
	#$current_major=$2;
	#$current_minor=$3;
	#$current_revision=$4;
	#$current_age=$5;

	printf("Current tag: $current_tag\n\tprefix: $current_prefix\n\t major: $current_major\n\t minor: $current_minor\n\t   rev: $current_revision\n\t   age: $current_age\n");

	# **********************************
	# Compare the last tag with the current source
	# **********************************

	unless (defined $increment) {
		printf("Diffing...\n");

		system("cvs diff -u -kk -r $current_tag $module | less");
	}

	# **********************************
	# Generate the new tag name
	# **********************************

	printf("\nWhich component do you wish to increment?\n\n\t'j'\tmaJor\n\t'i'\tmInor\n\t'r'\tRevision\n\t'a'\tAge\n\tfree type\tUse what I have typed (single word) as a new age name (original: $current_age)\n\nType in your choice: ");

	unless (defined $increment) {
		$increment=<STDIN>;
	}

	chomp($increment);

	switch ($increment) {
		case "j" {
			$major=$current_major+1;
			$minor=0;
			$revision=0;
			$age=1;}
		case "i" {
			$major=$current_major;
			$minor=$current_minor+1;
			$revision=0;
			$age=1;}
		case "r" {
			$major=$current_major;
			$minor=$current_minor;
			$revision=$current_revision+1;
			$age=1;} 
		case "a" {
			$major=$current_major;
			$minor=$current_minor;
			$revision=$current_revision;
			$age=$current_age+1;} 
		else {
			$major=$current_major;
			$minor=$current_minor;
			$revision=$current_revision;
			$age=$increment;}
	}
	$tag="$current_prefix" . "$major" . "_$minor" . "_$revision" . "_$age";

	printf("\nNew tag: $tag\n\n");

	die "This tag already exists; reported by assertion" unless system("cvs log -h $module/Makefile | grep \"$tag\"");

	# **********************************
	# Create the execution script
	# **********************************

	open EXEC, ">", "./etics-tag-$module.$major.$minor.$revision-$age.sh" or die $!;

	printf (EXEC "#This script registers tags for the $module module, version $major.$minor.$revision-$age\n#Generated automatically by $0\n\n"); 


	# **********************************
	# Update the ChangeLog
	# **********************************

	if (-r "$module/project/ChangeLog") { # ChangeLog exists (where expected). Proceed.

		$tmpChangeLog="$TMPDIR/$module.ChangeLog.$$";

		system("cp $module/project/ChangeLog $tmpChangeLog");

		system("echo $major.$minor.$revision-$age >> $tmpChangeLog");

		$ChangeLogRet=system("vim $tmpChangeLog");

		printf("Modified ChangeLog ready, ret code: $ChangeLogRet\n");

		printf(EXEC "#Update and commit the ChangeLog\ncp $tmpChangeLog $module/project/ChangeLog\ncvs commit -m \"Appended the description of changes regarding version $major.$minor.$revision-$age\" $module/project/ChangeLog\n\n");

	}	

	# **********************************
	# Update version.properties
	# **********************************
        open V, "$module/project/version.properties" or die "$module/project/version.properties: $?\n";

	printf(EXEC "#Generate new version.properties\ncat >$module/project/version.properties <<EOF\n");
        while ($_ = <V>) {
                chomp;

		$_=~s/module\.version\s*=\s*[.0-9]+/module\.version=$major.$minor.$revision/;
                $_=~s/module\.age\s*=\s*(\S+)/module\.age=$age/;

		printf(EXEC "$_\n");
        }
        close V;
	printf(EXEC "EOF\n\n");
	printf(EXEC "cvs commit -m \"Modified to reflect version $major.$minor.$revision-$age\" $module/project/version.properties\n\n");


	$cwd=`pwd`;
	chomp($cwd);

	printf(EXEC "#Register the new tag\ncd $module\ncvs tag \"$tag\"\ncd \"$cwd\"\n");


	# **********************************
	# Etics configuration prepare / modify / upload
	# **********************************

	$currentconfig="$module_$module" . "_R_$current_major" . "_$current_minor" . "_$current_revision" . "_$current_age";
	$currentconfig=~s/^org.//;
	$currentconfig=~s/\./-/g;
	$newconfig="$module_$module" . "_R_$major" . "_$minor" . "_$revision" . "_$age";
	$newconfig=~s/^org.//;
	$newconfig=~s/\./-/g;

	$module=~/([^\.]+?)\.([^\.]+?)$/;
	$subsysname=$1;
	$modulename=$2;

	printf("Module=$module\nname=$modulename\nsubsys=$subsysname\n");
	printf("PATH=\$PATH:./org.glite.lb:./ configure --mode=etics --module $modulename --output $TMPDIR/$newconfig.ini.$$ --version $major.$minor.$revision-$age\n");
	system("PATH=\$PATH:./org.glite.lb:./ configure --mode=etics --module $modulename --output $TMPDIR/$newconfig.ini.$$ --version $major.$minor.$revision-$age");

#	printf("\nCurrent configuration:\t$currentconfig\nNew configuration:\t$newconfig\n\nPreparing...\n");
#
	if (defined $opt_g) {
		system("etics-configuration prepare -o $TMPDIR/$currentconfig.ini.$$ -c $currentconfig $module");
	}

#	open OLDCONF, "$TMPDIR/$currentconfig.ini.$$" or die $!; 
#	open NEWCONF, ">", "$TMPDIR/$newconfig.ini.$$" or die $!;

#       while ($_ = <OLDCONF>) {
#                chomp;

##                $_=~s/module\.age\s*=\s*(\S+)/module\.age=$age/;
#		$_=~s/$currentconfig/$newconfig/;
#		$_=~s/^\s*version\s*=\s*[.0-9]+/version = $major.$minor.$revision/;
#                $_=~s/^\s*age\s*=\s*\S+/age = $age/;

#		printf(NEWCONF "$_\n");
#        }

#	close(OLDCONF);
#	close(NEWCONF);

	printf(EXEC "\n#Add new configuration\netics-configuration add -i $TMPDIR/$newconfig.ini.$$ -c $newconfig $module\n");


	# **********************************
	# Final bows
	# **********************************

	close(EXEC);

	system("chmod +x \"./etics-tag-$module.$major.$minor.$revision-$age.sh\"");

	printf("\n\n---------\nFinished!\n\nExecution script written in:\t./etics-tag-$module.$major.$minor.$revision-$age.sh\nChangeLog candidate written in:\t$tmpChangeLog\n");
	printf("Old configuration stored in:\t$TMPDIR/$currentconfig.ini.$$\n") if (defined $opt_g);
	printf("New configuration written in:\t$TMPDIR/$newconfig.ini.$$\n\n");

