import Head from "@docusaurus/Head";
import Link from "@docusaurus/Link";
import useBaseUrl from "@docusaurus/useBaseUrl";
import useDocusaurusContext from "@docusaurus/useDocusaurusContext";
import {
  CheckCircleIcon,
  CpuChipIcon,
  SignalIcon,
  WifiIcon,
} from "@heroicons/react/24/outline";
import Layout from "@theme/Layout";

function Home() {
  const context = useDocusaurusContext();
  const { siteConfig } = context;
  return (
    <Layout>
      <Head>
        <meta property="og:title" content="Drone Detector" />
        <meta
          property="og:description"
          content="An open source drone detection system using T-Beam S3-Core and RadioLib"
        />
      </Head>
      <header class="hero hero--primary text-center p-2">
        <div class="container">
          <h1 class="hero__title text-4xl font-bold py-8">
            🛸 Drone Detector
          </h1>
          <p class="hero__subtitle">{siteConfig.tagline}</p>
          <div class="indexCtas">
            <Link class="button button--lg" to="/docs/introduction">
              Learn More
            </Link>
            <Link class="button button--lg" to="/docs/getting-started">
              Get Started
            </Link>
          </div>
        </div>
      </header>

      <main class="flex flex-col gap-8">
        <div class="container mx-auto flex w-auto flex-col gap-4 mt-8">
          <h3 class="text-xl font-bold">Features</h3>
          <div class="flex flex-wrap justify-center gap-6">
            <div class="flex flex-col items-center">
              <div class="flex items-center gap-2">
                <SignalIcon
                  class="h-8 w-8"
                  style={{ color: "var(--ifm-color-primary)" }}
                />
                <h4 class="text-lg font-bold">Multi-Frequency Scanning</h4>
              </div>
              <p class="text-center max-w-xs">
                Scan across 433 MHz, 868 MHz, and 915 MHz frequencies using the SX1262 LoRa transceiver.
              </p>
            </div>
            <div class="flex flex-col items-center">
              <div class="flex items-center gap-2">
                <CpuChipIcon
                  class="h-8 w-8"
                  style={{ color: "var(--ifm-color-primary)" }}
                />
                <h4 class="text-lg font-bold">T-Beam S3-Core Hardware</h4>
              </div>
              <p class="text-center max-w-xs">
                Built on the reliable LILYGO T-Beam S3-Core with ESP32-S3, GPS, and LoRa capabilities.
              </p>
            </div>
            <div class="flex flex-col items-center">
              <div class="flex items-center gap-2">
                <WifiIcon
                  class="h-8 w-8"
                  style={{ color: "var(--ifm-color-primary)" }}
                />
                <h4 class="text-lg font-bold">RadioLib Integration</h4>
              </div>
              <p class="text-center max-w-xs">
                Powered by the versatile RadioLib library for comprehensive RF protocol support.
              </p>
            </div>
          </div>
        </div>

        <div class="container mx-auto flex w-auto flex-col gap-4">
          <h3 class="text-xl font-bold">Getting Started</h3>
          <div class="flex flex-wrap justify-center gap-6">
            <div class="flex flex-col items-center">
              <div class="flex items-center gap-2">
                <CheckCircleIcon
                  class="h-8 w-8"
                  style={{ color: "var(--ifm-color-primary)" }}
                />
                <h4 class="text-lg font-bold">Step 1</h4>
              </div>
              <Link
                to="/docs/hardware"
                class="mt-1 underline hover:no-underline font-semibold"
              >
                Get the Hardware &rarr;
              </Link>
              <p class="text-center max-w-xs">
                Acquire a T-Beam S3-Core device from LILYGO.
              </p>
            </div>
            <div class="flex flex-col items-center">
              <div class="flex items-center gap-2">
                <CheckCircleIcon
                  class="h-8 w-8"
                  style={{ color: "var(--ifm-color-primary)" }}
                />
                <h4 class="text-lg font-bold">Step 2</h4>
              </div>
              <Link
                to="/docs/getting-started"
                class="mt-1 underline hover:no-underline font-semibold"
              >
                Flash the Firmware &rarr;
              </Link>
              <p class="text-center max-w-xs">
                Flash your device with the Drone Detector firmware.
              </p>
            </div>
            <div class="flex flex-col items-center">
              <div class="flex items-center gap-2">
                <CheckCircleIcon
                  class="h-8 w-8"
                  style={{ color: "var(--ifm-color-primary)" }}
                />
                <h4 class="text-lg font-bold">Step 3</h4>
              </div>
              <Link
                to="/docs/usage"
                class="mt-1 underline hover:no-underline font-semibold"
              >
                Start Detecting &rarr;
              </Link>
              <p class="text-center max-w-xs">
                Use the button interface to navigate menus and start scanning for drone signals.
              </p>
            </div>
          </div>
        </div>
        <br />
      </main>
    </Layout>
  );
}

export default Home;
