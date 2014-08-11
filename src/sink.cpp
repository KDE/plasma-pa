#include "sink.h"

#include <QDebug>

void Sink::setInfo(const pa_sink_info *info)
{
//    const char *name;                  /**< Name of the sink */
//    uint32_t index;                    /**< Index of the sink */
//    const char *description;           /**< Description of this sink */
//    pa_sample_spec sample_spec;        /**< Sample spec of this sink */
//    pa_channel_map channel_map;        /**< Channel map */
//    uint32_t owner_module;             /**< Index of the owning module of this sink, or PA_INVALID_INDEX. */
//    pa_cvolume volume;                 /**< Volume of the sink */
//    int mute;                          /**< Mute switch of the sink */
//    uint32_t monitor_source;           /**< Index of the monitor source connected to this sink. */
//    const char *monitor_source_name;   /**< The name of the monitor source. */
//    pa_usec_t latency;                 /**< Length of queued audio in the output buffer. */
//    const char *driver;                /**< Driver name */
//    pa_sink_flags_t flags;             /**< Flags */
//    pa_proplist *proplist;             /**< Property list \since 0.9.11 */
//    pa_usec_t configured_latency;      /**< The latency this device has been configured to. \since 0.9.11 */
//    pa_volume_t base_volume;           /**< Some kind of "base" volume that refers to unamplified/unattenuated volume in the context of the output device. \since 0.9.15 */
//    pa_sink_state_t state;             /**< State \since 0.9.15 */
//    uint32_t n_volume_steps;           /**< Number of volume steps for sinks which do not support arbitrary volumes. \since 0.9.15 */
//    uint32_t card;                     /**< Card index, or PA_INVALID_INDEX. \since 0.9.15 */
//    uint32_t n_ports;                  /**< Number of entries in port array \since 0.9.16 */
//    pa_sink_port_info** ports;         /**< Array of available ports, or NULL. Array is terminated by an entry set to NULL. The number of entries is stored in n_ports. \since 0.9.16 */
//    pa_sink_port_info* active_port;    /**< Pointer to active port in the array, or NULL. \since 0.9.16 */
//    uint8_t n_formats;                 /**< Number of formats supported by the sink. \since 1.0 */
//    pa_format_info **formats;          /**< Array of formats supported by the sink. \since 1.0 */

    m_name = info->name;
    m_index = info->index;
    m_description = info->description;

#warning fixme channels
    m_volume = info->volume;

    qDebug() << info->name << info->volume.values[0] << info->driver;
}
