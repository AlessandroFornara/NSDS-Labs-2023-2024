package it.polimi.nsds.kafka;

import org.apache.kafka.clients.consumer.ConsumerConfig;
import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.kafka.clients.consumer.ConsumerRecords;
import org.apache.kafka.clients.consumer.KafkaConsumer;
import org.apache.kafka.common.serialization.IntegerDeserializer;
import org.apache.kafka.common.serialization.StringDeserializer;

import java.time.Duration;
import java.time.temporal.ChronoUnit;
import java.util.Collections;
import java.util.HashMap;
import java.util.Properties;

public class PopularTopicConsumer {
    private static final String topic = "inputTopic";
    private static final String serverAddr = "localhost:9092";

    public static void main(String[] args) {
        String groupId = args[0];

        final Properties props = new Properties();
        props.put(ConsumerConfig.BOOTSTRAP_SERVERS_CONFIG, serverAddr);
        props.put(ConsumerConfig.GROUP_ID_CONFIG, groupId);

        // TODO: complete the implementation by adding code here
        HashMap<String, Integer> overallNumber = new HashMap<>();
        props.put(ConsumerConfig.ENABLE_AUTO_COMMIT_CONFIG, String.valueOf(true));
        props.put(ConsumerConfig.AUTO_COMMIT_INTERVAL_MS_CONFIG, String.valueOf(15000));

        props.put(ConsumerConfig.AUTO_OFFSET_RESET_CONFIG, "latest");

        props.put(ConsumerConfig.KEY_DESERIALIZER_CLASS_CONFIG, StringDeserializer.class.getName());
        props.put(ConsumerConfig.VALUE_DESERIALIZER_CLASS_CONFIG, IntegerDeserializer.class.getName());

        KafkaConsumer<String, Integer> consumer = new KafkaConsumer<>(props);
        consumer.subscribe(Collections.singletonList(topic));

        final Integer possibleKeys = 1000;
        Integer maxValue = 0;
        int[] occurrences = new int[possibleKeys];
        for (int i = 0; i < possibleKeys; i++) {
            occurrences[i] = 0;
        }

        while (true) {
            final ConsumerRecords<String, Integer> records = consumer.poll(Duration.of(5, ChronoUnit.MINUTES));
            for (final ConsumerRecord<String, Integer> record : records) {
                final String currentKey = record.key();
                final String cleanKey = currentKey.substring(3);        // to exclude "Key" characters
                Integer numKey = Integer.parseInt(cleanKey);
                occurrences[numKey]++;
                // System.out.println("Updated: " + occurrences[numKey] + "with Key" + numKey + ", maxValue: " + maxValue);

                if (occurrences[numKey] > maxValue)
                    maxValue = occurrences[numKey];

                for (int i = 0; i < possibleKeys; i++) {
                    if (occurrences[i] == maxValue) {
                        System.out.print("Key" + i + " ");
                    }
                }
                System.out.println();
            }
        }
    }
}
