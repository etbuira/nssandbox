/* Copyright Etienne Buira
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>
#include <string.h>
#include <grp.h>
#include <pwd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>

struct opts {
	unsigned int ns_net:1;
	unsigned int ns_ipc:1;
	char **command;
};

static int run_cmd(struct opts const * const opts)
{
	fflush(NULL);

	if (execvp(opts->command[0], opts->command)) {
		perror("execvp");
		return -1;
	}

	return -1;
}

static int drop_privileges(void)
{
	uid_t uid;
	gid_t gid;

	gid = getgid();

	if (setgid(gid)) {
		perror("setgid");
		return -1;
	}

	uid = getuid();

	if (setuid(uid)) {
		perror("setuid");
		return -1;
	}

	return 0;
}

static int build_env_net(void)
{
	int sock;
	struct ifreq loup;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		return -1;

	memset(&loup, 0, sizeof(loup));
	strncpy(loup.ifr_name, "lo", sizeof(loup.ifr_name));
	loup.ifr_flags = IFF_UP;

	if (ioctl(sock, SIOCSIFFLAGS, &loup)) {
		perror("ioctl");
		close(sock);
		return -1;
	}

	close(sock);

	return 0;
}

static int env_flags(struct opts const * const opts)
{
	int flags = 0;

	if (opts->ns_net)
		flags |= CLONE_NEWNET;

	if (opts->ns_ipc)
		flags |= CLONE_NEWIPC;

	return flags;
}

static int enter_env(struct opts const * const opts)
{
	int r;

	if (unshare(env_flags(opts))) {
		int r = errno;
		perror("unshare");
		if (r == EINVAL)
			fprintf(stderr, "Please make sure you compiled your kernel with CONFIG_IPC_NS and/or CONFIG_NET_NS\n");
		return -1;
	}

	if (opts->ns_net) {
		if ((r = build_env_net()))
			return r;
	}

	return 0;
}

static void print_usage(char const * const progname)
{
	fprintf(stderr, "%s [options] cmd args\n", progname);
	fprintf(stderr, "\n");
	fprintf(stderr, "\t-h: display this help message\n");
	fprintf(stderr, "\t-i: unshare IPC namespace\n");
	fprintf(stderr, "\t-n: unshare network namespace, and bring lo up\n");
}

static int parse_options(struct opts *opts, int argc, char *argv[])
{
	int opt;
	size_t ncmd, i;

	while ((opt = getopt(argc, argv, "+hin")) != -1) {
		switch(opt) {
			case 'h':
				print_usage(argv[0]);
				return -1;
			case 'i':
				opts->ns_ipc = 1;
				break;
			case 'n':
				opts->ns_net = 1;
				break;
			case '?':
			default:
				fprintf(stderr, "%c option is unhandled\n", optopt);
				return -1;
		}
	}

	if (optind == argc) {
		fprintf(stderr, "No command were provided\n");
		return -1;
	}
	ncmd = argc-optind+1;
	if (SIZE_MAX/sizeof(opts->command[0]) < ncmd) {
		fprintf(stderr, "Command line too long\n");
		return -1;
	}
	if (!( opts->command = calloc(ncmd, sizeof(opts->command[0])) )) {
		fprintf(stderr, "Could not allocate memory\n");
		return -1;
	}
	for (i=0 ; i<ncmd ; i++)
		opts->command[i] = argv[optind+i];

	return 0;
}

int main(int argc, char *argv[])
{
	struct opts opts = { 0 };

	if (parse_options(&opts, argc, argv))
		return EXIT_FAILURE;

	if (enter_env(&opts))
		return EXIT_FAILURE;

	if (drop_privileges())
		return EXIT_FAILURE;

	if (run_cmd(&opts))
		return EXIT_FAILURE;

	return EXIT_FAILURE;
}

