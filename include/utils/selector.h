#ifndef SELECTOR_H_W50GNLODsARolpHbsDsrvYvMsbT
#define SELECTOR_H_W50GNLODsARolpHbsDsrvYvMsbT

#include <sys/time.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * selector.c - un muliplexor de entrada salida
 *
 * Un selector permite manejar en un único hilo de ejecución la entrada salida
 * de file descriptors de forma no bloqueante.
 *
 * Esconde la implementación final (select(2) / poll(2) / epoll(2) / ..)
 *
 * El usuario registra para un file descriptor especificando:
 *  1. un handler: provee funciones callback que manejarán los eventos de
 *     entrada/salida
 *  2. un interés: que especifica si interesa leer o escribir.
 *
 * Es importante que los handlers no ejecute tareas bloqueantes ya que demorará
 * el procesamiento del resto de los descriptores.
 *
 * Si el handler requiere bloquearse por alguna razón (por ejemplo realizar
 * una resolución de DNS utilizando getaddrinfo(3)), tiene la posiblidad de
 * descargar el trabajo en un hilo notificará al selector que el resultado del
 * trabajo está disponible y se le presentará a los handlers durante
 * la iteración normal. Los handlers no se tienen que preocupar por la
 * concurrencia.
 *
 * Dicha señalización se realiza mediante señales, y es por eso que al
 * iniciar la librería 'selector_init' se debe configurar una señal a utilizar.
 *
 * Todos métodos retornan su estado (éxito / error) de forma uniforme.
 * Puede utilizar 'selector_error' para obtener una representación human
 * del estado. Si el valor es `SELECTOR_IO' puede obtener información adicional
 * en errno(3).
 *
 * El flujo de utilización de la librería es:
 *  - iniciar la libreria `selector_init'
 *  - crear un selector: `selector_new'
 *  - registrar un file descriptor: `selector_register_fd'
 *  - esperar algún evento: `selector_iteratate'
 *  - destruir los recursos de la librería `selector_close'
 */
typedef struct fdselector * fd_selector;

/** valores de retorno. */
typedef enum {
    /** llamada exitosa */
    SELECTOR_SUCCESS  = 0,
    /** no pudimos alocar memoria */
    SELECTOR_ENOMEM   = 1,
    /** llegamos al límite de fds que la plataforma puede manejar */
    SELECTOR_MAXFD    = 2,
    /** argumento invalido */
    SELECTOR_IARGS    = 3,
    /** descriptor ya está en uso */
    SELECTOR_FDINUSE  = 4,
    /** I/O error check errno */
    SELECTOR_IO       = 5,
    /** fallo de time */
    SELECTOR_TIME     = 6,
} selector_status;

typedef enum { 
    NO_TIMEOUT, 
    CON_TIMEOUT, 
    GEN_TIMEOUT, 
} selector_timeout;

/** retorna una descripción humana del fallo */
const char *
selector_error(const selector_status status);

/** opciones de inicialización del selector */
struct selector_init {
    /** señal a utilizar para notificaciones internas */
    const int signal;

    /** tiempo máximo de bloqueo durante 'selector_iteratate' */
    struct timespec select_timeout;
};

/** inicializa la librería */
selector_status
selector_init(const struct selector_init *c);

/** deshace la incialización de la librería */
selector_status
selector_close(void);

/* instancia un nuevo selector. returna NULL si no puede instanciar  */
fd_selector
selector_new(const size_t initial_elements);

/** destruye un selector creado por _new. Tolera NULLs */
void
selector_destroy(fd_selector s);

/**
 * Intereses sobre un file descriptor (quiero leer, quiero escribir, …)
 *
 * Son potencias de 2, por lo que se puede requerir una conjunción usando el OR
 * de bits.
 *
 * OP_NOOP es útil para cuando no se tiene ningún interés.
 */
typedef enum {
    OP_NOOP    = 0,
    OP_READ    = 1 << 0,
    OP_WRITE   = 1 << 2,
} fd_interest ;

/**
 * Quita un interés de una lista de intereses
 */
#define INTEREST_OFF(FLAG, MASK)  ( (FLAG) & ~(MASK) )

/**
 * Argumento de todas las funciones callback del handler
 */
typedef struct selector_key {
    /** el selector que dispara el evento */
    fd_selector s;
    /** el file descriptor en cuestión */
    int         fd;
    /** dato provisto por el usuario */
    void *      data;
} selector_key;

/**
 * Manejador de los diferentes eventos..
 */
typedef struct fd_handler {
  void (*handle_read)      (struct selector_key *key);
  void (*handle_write)     (struct selector_key *key);
  void (*handle_block)     (struct selector_key *key);

  /**
   * llamado cuando se desregistra el fd
   * Seguramente deba liberar los recusos alocados en data.
   */
  void (*handle_close)     (struct selector_key *key);
  /* llamado cuando salta un timeout de tipo CON_TIMEOUT  */
  void (*handle_timeout)   (struct selector_key *key);
} fd_handler;

/**
 * registra en el selector 's' un nuevo file descriptor `fd'.
 *
 * Se especifica un 'interest' inicial, y se pasa handler que manejará
 * los diferentes eventos. 'data' es un adjunto que se pasa a todos
 * los manejadores de eventos.
 *
 * No se puede registrar dos veces un mismo fd.
 *
 * @return 0 si fue exitoso el registro.
 */
selector_status
selector_register(fd_selector        s,
                  const int          fd,
                  const fd_handler  *handler,
                  const fd_interest  interest,
                  void              *data,
                  selector_timeout   timeout);

/**
 * desregistra un file descriptor del selector
 */
selector_status
selector_unregister_fd(fd_selector   s,
                       const int     fd);

/** permite cambiar los intereses para un file descriptor */
selector_status
selector_set_interest(fd_selector s, int fd, fd_interest i);

/** permite cambiar los intereses para un file descriptor */
selector_status
selector_set_interest_key(struct selector_key *key, fd_interest i);

/** permite obtener los intereses de un file descriptor */
selector_status
selector_get_interest(fd_selector s, int fd, fd_interest *i);

/** permite obtener los intereses de un file descriptor */
selector_status
selector_get_interest_key(struct selector_key *key, fd_interest *i);

/** permite agregar un interes a un file descriptor */
selector_status
selector_add_interest(fd_selector s, int fd, fd_interest i);

/** permite eliminar un interes de un file descriptor */
selector_status
selector_remove_interest(fd_selector s, int fd, fd_interest i);

/**
 * se bloquea hasta que hay eventos disponible y los despacha.
 * Retorna luego de cada iteración, o al llegar al timeout.
 */
selector_status
selector_select(fd_selector s);

/**
 * Método de utilidad que activa O_NONBLOCK en un fd.
 *
 * retorna -1 ante error, y deja detalles en errno.
 */
int
selector_fd_set_nio(const int fd);

/** notifica que un trabajo bloqueante terminó */
selector_status
selector_notify_block(fd_selector s,
                 const int   fd);

/** 
 * Recorre los fds en uso de un selector y revisa si alguno 
 * esta inactivo hace al menos timeout segundos (segun corresponda). 
 * De ser asi, lo desregistra y lo cierra
*/
void selector_check_timeout(fd_selector s, time_t timeout_gen, time_t timeout_con);

/* Modifica la opcion de timeout para un fd */
selector_status
selector_set_timeout_option(fd_selector s, int fd, selector_timeout timeout);

#endif