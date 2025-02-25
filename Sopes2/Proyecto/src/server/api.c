#include <ulfius.h>
#include <jansson.h>
#include <sys/syscall.h>
#include <errno.h>
#include <string.h>

#define PORT 5000

// Definir las estructuras según las syscalls
struct memory_info {
    unsigned long free_memory;
    unsigned long used_memory;
    unsigned long cached_memory;
};

struct swap_info {
    unsigned long free_pages;
    unsigned long used_pages;
};

struct page_fault_info {
    unsigned long minor_faults;
    unsigned long major_faults;
};

struct page_status_info {
    unsigned long active_pages;
    unsigned long inactive_pages;
};

struct process_memory_info {
    pid_t pid;
    char comm[16];
    unsigned long rss;
};

struct top_processes {
    struct process_memory_info processes[5];
};

// Definir números de syscalls (ajustar según sea necesario)
#define SYS_my_meminfo 554
#define SYS_my_swapinfo 555
#define SYS_GET_PAGE_FAULTS 556
#define SYS_GET_PAGE_STATUS 557
#define SYS_GET_TOP_PROCESSES 558

// Función para agregar CORS a la respuesta
void add_cors_headers(struct _u_response *response) {
    u_map_put(response->map_header, "Access-Control-Allow-Origin", "*");  // Permitir todos los orígenes
    u_map_put(response->map_header, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    u_map_put(response->map_header, "Access-Control-Allow-Headers", "Content-Type");
}

// Funciones para manejar cada syscall y devolver el resultado en JSON
json_t* get_memory_info() {
    struct memory_info info;
    long res = syscall(SYS_my_meminfo, &info);
    
    if (res != 0) {
        return NULL;
    }

    json_t *json = json_object();
    json_object_set_new(json, "free_memory", json_integer(info.free_memory));
    json_object_set_new(json, "used_memory", json_integer(info.used_memory));
    json_object_set_new(json, "cached_memory", json_integer(info.cached_memory));

    return json;
}

json_t* get_swap_info() {
    struct swap_info info;
    long res = syscall(SYS_my_swapinfo, &info);

    if (res != 0) {
        return NULL;
    }

    json_t *json = json_object();
    json_object_set_new(json, "free_pages", json_integer(info.free_pages));
    json_object_set_new(json, "used_pages", json_integer(info.used_pages));

    return json;
}

json_t* get_page_faults() {
    struct page_fault_info info;
    long res = syscall(SYS_GET_PAGE_FAULTS, &info);

    if (res != 0) {
        return NULL;
    }

    json_t *json = json_object();
    json_object_set_new(json, "minor_faults", json_integer(info.minor_faults));
    json_object_set_new(json, "major_faults", json_integer(info.major_faults));

    return json;
}

json_t* get_page_status() {
    struct page_status_info info;
    long res = syscall(SYS_GET_PAGE_STATUS, &info);

    if (res != 0) {
        return NULL;
    }

    json_t *json = json_object();
    json_object_set_new(json, "active_pages", json_integer(info.active_pages));
    json_object_set_new(json, "inactive_pages", json_integer(info.inactive_pages));

    return json;
}

json_t* get_top_processes() {
    struct top_processes info;
    long res = syscall(SYS_GET_TOP_PROCESSES, &info);

    if (res != 0) {
        return NULL;
    }

    json_t *json = json_object();
    json_t *processes_array = json_array();

    for (int i = 0; i < 5; i++) {
        json_t *process_json = json_object();
        json_object_set_new(process_json, "pid", json_integer(info.processes[i].pid));
        json_object_set_new(process_json, "comm", json_string(info.processes[i].comm));
        json_object_set_new(process_json, "rss", json_integer(info.processes[i].rss / 1024)); // En KB
        json_array_append_new(processes_array, process_json);
    }

    json_object_set_new(json, "processes", processes_array);

    return json;
}

// Manejadores de rutas con CORS
int callback_get_memory_info(const struct _u_request *request, struct _u_response *response, void *user_data) {
    add_cors_headers(response);
    json_t *json = get_memory_info();
    
    if (json) {
        ulfius_set_json_body_response(response, 200, json);
        json_decref(json);
    } else {
        ulfius_set_string_body_response(response, 500, "Error obteniendo información de memoria");
    }

    return U_CALLBACK_CONTINUE;
}

int callback_get_swap_info(const struct _u_request *request, struct _u_response *response, void *user_data) {
    add_cors_headers(response);
    json_t *json = get_swap_info();

    if (json) {
        ulfius_set_json_body_response(response, 200, json);
        json_decref(json);
    } else {
        ulfius_set_string_body_response(response, 500, "Error obteniendo información de swap");
    }

    return U_CALLBACK_CONTINUE;
}

int callback_get_page_faults(const struct _u_request *request, struct _u_response *response, void *user_data) {
    add_cors_headers(response);
    json_t *json = get_page_faults();

    if (json) {
        ulfius_set_json_body_response(response, 200, json);
        json_decref(json);
    } else {
        ulfius_set_string_body_response(response, 500, "Error obteniendo fallos de página");
    }

    return U_CALLBACK_CONTINUE;
}

int callback_get_page_status(const struct _u_request *request, struct _u_response *response, void *user_data) {
    add_cors_headers(response);
    json_t *json = get_page_status();

    if (json) {
        ulfius_set_json_body_response(response, 200, json);
        json_decref(json);
    } else {
        ulfius_set_string_body_response(response, 500, "Error obteniendo estado de páginas");
    }

    return U_CALLBACK_CONTINUE;
}

int callback_get_top_processes(const struct _u_request *request, struct _u_response *response, void *user_data) {
    add_cors_headers(response);
    json_t *json = get_top_processes();

    if (json) {
        ulfius_set_json_body_response(response, 200, json);
        json_decref(json);
    } else {
        ulfius_set_string_body_response(response, 500, "Error obteniendo procesos");
    }

    return U_CALLBACK_CONTINUE;
}

// Manejador para solicitudes OPTIONS (Preflight CORS)
int callback_options(const struct _u_request *request, struct _u_response *response, void *user_data) {
    add_cors_headers(response);
    ulfius_set_empty_body_response(response, 204); // No Content
    return U_CALLBACK_CONTINUE;
}

// Main de la API
int main(void) {
    struct _u_instance instance;

    // Inicializar instancia de ulfius
    if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
        fprintf(stderr, "Error inicializando la instancia de Ulfius\n");
        return 1;
    }

    // Registrar manejadores para solicitudes OPTIONS (Preflight CORS)
    ulfius_add_endpoint_by_val(&instance, "OPTIONS", "*", NULL, 0, &callback_options, NULL);

    // Registrar rutas para las syscalls con CORS
    ulfius_add_endpoint_by_val(&instance, "GET", "/memory_info", NULL, 0, &callback_get_memory_info, NULL);
    ulfius_add_endpoint_by_val(&instance, "GET", "/swap_info", NULL, 0, &callback_get_swap_info, NULL);
    ulfius_add_endpoint_by_val(&instance, "GET", "/page_faults", NULL, 0, &callback_get_page_faults, NULL);
    ulfius_add_endpoint_by_val(&instance, "GET", "/page_status", NULL, 0, &callback_get_page_status, NULL);
    ulfius_add_endpoint_by_val(&instance, "GET", "/top_processes", NULL, 0, &callback_get_top_processes, NULL);

    // Iniciar el framework de Ulfius
    if (ulfius_start_framework(&instance) == U_OK) {
        printf("Servidor iniciado en el puerto %d\n", PORT);
        getchar();  // Espera a que el usuario presione una tecla
    } else {
        fprintf(stderr, "Error iniciando el framework de Ulfius\n");
    }

    // Detener el framework y limpiar
    ulfius_stop_framework(&instance);
    ulfius_clean_instance(&instance);

    return 0;
}
