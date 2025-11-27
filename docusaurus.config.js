// @ts-check

require("dotenv").config();

/** @type {import('@docusaurus/types').Config} */
const config = {
  title: "Drone Detector",
  tagline:
    "An open source drone detection system using T-Beam S3-Core and RadioLib for sub-GHz RF scanning",
  url: "https://drone-detector.org",
  baseUrl: "/",
  trailingSlash: true,
  onBrokenLinks: "throw",
  onBrokenMarkdownLinks: "warn",
  favicon: "design/web/favicon.ico",
  organizationName: "drone-detector",
  projectName: "drone-detector",
  themeConfig: /** @type {import('@docusaurus/preset-classic').ThemeConfig} */ {
    docs: {
      sidebar: {
        autoCollapseCategories: true,
      },
    },
    navbar: {
      hideOnScroll: true,
      title: "Drone Detector",
      items: [
        {
          label: "Docs",
          to: "docs/introduction",
        },
        {
          label: "About",
          position: "right",
          items: [
            {
              label: "Introduction",
              to: "docs/introduction",
            },
            {
              label: "Getting Started",
              to: "docs/getting-started",
            },
            {
              label: "Hardware",
              to: "docs/hardware",
            },
          ],
        },
        {
          href: "https://github.com/jgromes/RadioLib",
          position: "right",
          className: "header-github-link",
          "aria-label": "RadioLib GitHub",
        },
      ],
    },
    footer: {
      copyright: `Drone Detector Project | Built with T-Beam S3-Core and RadioLib`,
    },
    colorMode: {
      respectPrefersColorScheme: true,
    },
    mermaid: {
      theme: { light: "base", dark: "base" },
      options: {
        themeVariables: {
          primaryColor: "#67EA94",
          primaryTextColor: "var(--tw-prose-headings)",
          primaryBorderColor: "#4D4D4D",
          lineColor: "#EAD667",
          secondaryColor: "#EA67BD",
          tertiaryColor: "#677CEA",
        },
      },
    },
    prism: {
      additionalLanguages: ["shell-session", "bash", "cpp"],
    },
  },
  plugins: [
    () => {
      return {
        name: "docusaurus-tailwindcss",
        configurePostCss(postcssOptions) {
          postcssOptions.plugins.push(require("tailwindcss"));
          postcssOptions.plugins.push(require("autoprefixer"));
          return postcssOptions;
        },
      };
    },
  ],
  presets: [
    [
      "@docusaurus/preset-classic",
      /** @type {import('@docusaurus/preset-classic').Options} */
      {
        docs: {
          sidebarPath: require.resolve("./sidebars.js"),
          breadcrumbs: false,
        },
        blog: false,
        theme: {
          customCss: require.resolve("./src/css/custom.css"),
        },
      },
    ],
  ],
  i18n: {
    defaultLocale: "en",
    locales: ["en"],
  },
  markdown: {
    mermaid: true,
  },
  themes: ["@docusaurus/theme-mermaid"],
  future: {
    v4: {
      useCssCascadeLayers: false,
    },
  },
};

module.exports = config;
