#include <elogind/sd-bus.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int status = EXIT_SUCCESS;

	char *session_id = getenv("XDG_SESSION_ID");
	if (!session_id) {
		fprintf(stderr, "getenv() failed\n");
		return EXIT_FAILURE;
	}

	sd_bus *bus = NULL;
	int ret = sd_bus_default_system(&bus);
	if (ret < 0) {
		fprintf(stderr, "sd_bus_default_system() failed with %d\n",
			ret);
		return EXIT_FAILURE;
	}

	sd_bus_message *message = NULL;
	sd_bus_error error = SD_BUS_ERROR_NULL;
	ret = sd_bus_call_method(bus, "org.freedesktop.login1",
				 "/org/freedesktop/login1",
				 "org.freedesktop.login1.Manager", "GetSession",
				 &error, &message, "s", session_id);
	if (ret < 0) {
		fprintf(stderr, "sd_bus_call_method() failed with %s\n",
			error.message);
		status = EXIT_FAILURE;
		goto cleanup_bus;
	}
	char *path_temp = NULL;
	ret = sd_bus_message_read(message, "o", &path_temp);
	if (ret < 0) {
		fprintf(stderr, "sd_bus_message_read() failed with %s\n",
			error.message);
		status = EXIT_FAILURE;
		goto cleanup_bus;
	}
	char *path = calloc(128, sizeof(char));
	if (!path) {
		fprintf(stderr, "calloc() failed\n");
		status = EXIT_FAILURE;
		goto cleanup_bus;
	}
	strncpy(path, path_temp, 128);
	sd_bus_error_free(&error);
	sd_bus_message_unref(message);

	ret = sd_bus_call_method(bus, "org.freedesktop.login1", path,
				 "org.freedesktop.login1.Session", "Activate",
				 &error, &message, "");
	if (ret < 0) {
		fprintf(stderr, "sd_bus_call_method() failed with %s\n",
			error.message);
		status = EXIT_FAILURE;
		goto cleanup_path;
	}
	sd_bus_error_free(&error);
	sd_bus_message_unref(message);

	ret = sd_bus_call_method(bus, "org.freedesktop.login1", path,
				 "org.freedesktop.login1.Session",
				 "TakeControl", &error, &message, "b", 0);
	if (ret < 0) {
		fprintf(stderr, "sd_bus_call_method() failed with %s\n",
			error.message);
		status = EXIT_FAILURE;
		goto cleanup_path;
	}
	sd_bus_error_free(&error);
	sd_bus_message_unref(message);

	ret = sd_bus_call_method(bus, "org.freedesktop.login1", path,
				 "org.freedesktop.login1.Session", "SetType",
				 &error, &message, "s", "x11");
	if (ret < 0) {
		fprintf(stderr, "sd_bus_call_method() failed with %s\n",
			error.message);
		status = EXIT_FAILURE;
		goto cleanup_path;
	}
	sd_bus_error_free(&error);
	sd_bus_message_unref(message);

	ret = system(argv[1]);
	if (ret) {
		fprintf(stderr, "system() failed with %d\n", ret);
		status = EXIT_FAILURE;
	}

	ret = sd_bus_call_method(bus, "org.freedesktop.login1", path,
				 "org.freedesktop.login1.Session", "SetType",
				 &error, &message, "s", "tty");
	if (ret < 0) {
		fprintf(stderr, "sd_bus_call_method() failed with %s\n",
			error.message);
		status = EXIT_FAILURE;
		goto cleanup_path;
	}
	sd_bus_error_free(&error);
	sd_bus_message_unref(message);

	ret = sd_bus_call_method(bus, "org.freedesktop.login1", path,
				 "org.freedesktop.login1.Session",
				 "ReleaseControl", &error, &message, "");
	if (ret < 0) {
		fprintf(stderr, "sd_bus_call_method() failed with %s\n",
			error.message);
		status = EXIT_FAILURE;
	}

cleanup_path:
	free(path);
cleanup_bus:
	sd_bus_error_free(&error);
	sd_bus_message_unref(message);
	sd_bus_close(bus);

	return status;
}
