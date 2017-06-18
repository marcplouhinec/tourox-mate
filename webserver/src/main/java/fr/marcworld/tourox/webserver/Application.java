package fr.marcworld.tourox.webserver;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.EnableAutoConfiguration;
import org.springframework.boot.autoconfigure.domain.EntityScan;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.data.jpa.repository.config.EnableJpaRepositories;

/**
 * Application entry-point.
 *
 * @author Marc Plouhinec
 */
@EnableAutoConfiguration
@EntityScan(basePackages = "fr.marcworld.tourox.webserver.entity")
@EnableJpaRepositories("fr.marcworld.tourox.webserver.repository")
@ComponentScan("fr.marcworld.tourox.webserver.controller")
public class Application {

    public static void main(String[] args) throws Exception {
        SpringApplication.run(Application.class, args);
    }
}
